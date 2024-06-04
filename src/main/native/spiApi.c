/*
 * spiApi.c
 *
 * Copyright (c) 2014 Jeremy Garff <jer @ jers.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     1.  Redistributions of source code must retain the above copyright notice, this list of
 *         conditions and the following disclaimer.
 *     2.  Redistributions in binary form must reproduce the above copyright notice, this list
 *         of conditions and the following disclaimer in the documentation and/or other materials
 *         provided with the distribution.
 *     3.  Neither the name of the owner nor the names of its contributors may be used to endorse
 *         or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <signal.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <time.h>
#include <math.h>

//#include "mailbox.h"
//#include "gpio.h"
//#include "dma.h"
//#include "pwm.h"
//#include "pcm.h"
//#include "rpihw.h"

#include "spiApi.h"


#define BUS_TO_PHYS(x)                           ((x)&~0xC0000000)

#define OSC_FREQ                                 19200000   // crystal frequency
#define OSC_FREQ_PI4                             54000000   // Pi 4 crystal frequency

/* 4 colors (R, G, B + W), 8 bits per byte, 3 symbols per bit + 55uS low for reset signal */
#define LED_COLOURS                              4
#define LED_RESET_uS                             55
#define LED_BIT_COUNT(leds, freq)                ((leds * LED_COLOURS * 8 * 3) + ((LED_RESET_uS * \
                                                  (freq * 3)) / 1000000))

/* Minimum time to wait for reset to occur in microseconds. */
#define LED_RESET_WAIT_TIME                      300

// Pad out to the nearest uint32 + 32-bits for idle low/high times the number of channels
#define PWM_BYTE_COUNT(leds, freq)               (((((LED_BIT_COUNT(leds, freq) >> 3) & ~0x7) + 4) + 4) * \
                                                  RPI_PWM_CHANNELS)
#define PCM_BYTE_COUNT(leds, freq)               ((((LED_BIT_COUNT(leds, freq) >> 3) & ~0x7) + 4) + 4)

// Driver mode definitions
#define NONE	0
#define PWM	1
#define PCM	2
#define SPI	3

// We use the mailbox interface to request memory from the VideoCore.
// This lets us request one physically contiguous chunk, find its
// physical address, and map it 'uncached' so that writes from this
// code are immediately visible to the DMA controller.  This struct
// holds data relevant to the mailbox interface.
typedef struct videocore_mbox {
    int handle;             /* From mbox_open() */
    unsigned mem_ref;       /* From mem_alloc() */
    unsigned bus_addr;      /* From mem_lock() */
    unsigned size;          /* Size of allocation */
    uint8_t *virt_addr;     /* From mapmem() */
} videocore_mbox_t;




int spiApi_sum(int n1, int n2) {
  return n1+n2;
}




/**
 * Cleanup previously allocated device memory and buffers.
 *
 * @param    spiApi  spiApi instance pointer.
 *
 * @returns  None
 */
void spiApi_cleanup(spiApi_t *spiApi)
{
    spiApi_device_t *device = spiApi->device;


    if (device && (device->spi_fd > 0))
    {
        close(device->spi_fd);
    }

    if (device) {
        free(device);
    }
    spiApi->device = NULL;
}





static spiApi_return_t spi_transfer(spiApi_t *spiApi)
{
    int ret;
    struct spi_ioc_transfer tr;

    memset(&tr, 0, sizeof(struct spi_ioc_transfer));
    tr.tx_buf = 0;  // todo get buffer from spiApi (unsigned long)spiApi->device->pxl_raw;
    tr.rx_buf = 0;
    tr.len = 0;  // todo get buffer length from spiApi     >max_count, spiApi->freq);

    ret = ioctl(spiApi->device->spi_fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        fprintf(stderr, "Can't send spi message");
        return spiApi_ERROR_SPI_TRANSFER;
    }

    return spiApi_SUCCESS;
}


/*
 *
 * Application API Functions
 *
 */

static spiApi_return_t spi_init(spiApi_t *spiApi)
{
    int spi_fd;
    static uint8_t mode;
    static uint8_t bits = 8;
    uint32_t speed = spiApi->max_freq * 3;
    spiApi_device_t *device = spiApi->device;

       spi_fd = open("/dev/spidev0.0", O_RDWR);
    if (spi_fd < 0) {
        fprintf(stderr, "Cannot open /dev/spidev0.0     Review config.txt update\n");
        return spiApi_ERROR_SPI_SETUP;
    }

    fprintf(stderr, "C spiApi->max_freq    %d\n", spiApi->max_freq);
    fprintf(stderr, "C device->spi_fd    %d\n", device->spi_fd);
    fprintf(stderr, "C device->driver_mode    %d\n", device->driver_mode);
    fprintf(stderr, "C spiApi->device->driver_mode    %d\n", spiApi->device->driver_mode);




    device->spi_fd = spi_fd;
    spiApi->max_freq = 22;
    device->driver_mode = 33;

    fprintf(stderr, "Opened /dev/spidev0.0  FD %d \n " , spi_fd);
    fprintf(stderr, "Opened /dev/spidev0.0  Device FD %d \n " , device->spi_fd);
     // SPI mode
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0)
    {
        return spiApi_ERROR_SPI_SETUP;
    }
    if (ioctl(spi_fd, SPI_IOC_RD_MODE, &mode) < 0)
    {
        return spiApi_ERROR_SPI_SETUP;
    }

    // Bits per word
    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0)
    {
        return spiApi_ERROR_SPI_SETUP;
    }
    if (ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0)
    {
        return spiApi_ERROR_SPI_SETUP;
    }

    // Max speed Hz
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        return spiApi_ERROR_SPI_SETUP;
    }
    if (ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0)
    {
        return spiApi_ERROR_SPI_SETUP;
    }

    return spiApi_SUCCESS;
}

/**
 * Shut down DMA, PWM, and cleanup memory.
 *
 * @param    spiApi  spiApi instance pointer.
 *
 * @returns  None
 */
void spiApi_fini(spiApi_t *spiApi)
{

    spiApi_wait(spiApi);


    spiApi_cleanup(spiApi);
}

/**
 * Allocate and initialize memory, buffers, pages, PWM, DMA, and GPIO.
 *
 * @param    spiApi  spiApi instance pointer.
 *
 * @returns  0 on success, -1 otherwise.
 */
spiApi_return_t spiApi_init(spiApi_t *spiApi)
{
    spiApi_device_t *device = spiApi->device;

    fprintf(stderr," spiApi_init  \n");



    spiApi_return_t ret = spiApi_SUCCESS;

 // Use the structure allocated in the Java code
   // spiApi->device = malloc(sizeof(*spiApi->device));
    if (!spiApi->device)
    {
        return spiApi_ERROR_OUT_OF_MEMORY;
    }
   // memset(spiApi->device, 0, sizeof(*spiApi->device));
    device = spiApi->device;

    device->driver_mode = SPI;
     fprintf(stderr," B4 spiInit Driver Mode ???  %d \n", device->driver_mode);

    if (device->driver_mode == SPI) {
        fprintf(stderr," yes Driver in SPI Mode    \n");
        return spi_init(spiApi);
    }

    return ret;
}



/**
 * Wait for any executing DMA operation to complete before returning.
 *
 * @param    spiApi  spiApi instance pointer.
 *
 * @returns  0 on success, -1 on DMA competion error
 */
spiApi_return_t spiApi_wait(spiApi_t *spiApi)
{
 
    if (spiApi->device->driver_mode == SPI)  // Nothing to do for SPI
    {
        return spiApi_SUCCESS;
    }


    return spiApi_SUCCESS;
}


spiApi_return_t spiApi_render_test(spiApi_test_t *spiApitest){
  spiApi_return_t ret = spiApi_SUCCESS;
  int what = spiApitest->waldo;

 fprintf(stderr, "spiApi_test_t \n");
    unsigned char  *p = (unsigned char *)spiApitest;
    int i;
    for (i = 0; i < 32 ; i++)
        fprintf(stderr, "%02x ", p[i]);

  fprintf(stderr, "Before spiApi_render_testtt ran  %d    \n", what);

  what = 21;
  spiApitest->waldo = what;
  what = spiApitest->waldo;
  fprintf(stderr, "After spiApi_render_testttt  %d   RC  %d \n", what, ret);

 spiApitest->where =  spiApitest->where *2;
return(ret);
}


/**
 * Render the DMA buffer from the user supplied LED arrays and start the DMA
 * controller.  This will update all LEDs on both PWM channels.
 *
 * @param    spiApi  spiApi instance pointer.
 *
 * @returns  None
 */
spiApi_return_t  spiApi_render(spiApi_t *spiApi)
{

    spiApi_return_t ret = spi_transfer(spiApi);
   

    return ret;
}

const char * spiApi_get_return_t_str(const spiApi_return_t state)
{
    const int index = -state;
    static const char * const ret_state_str[] = { spiApi_RETURN_STATES(spiApi_RETURN_STATES_STRING) };

    if (index < (int)(sizeof(ret_state_str) / sizeof(ret_state_str[0])))
    {
        return ret_state_str[index];
    }

    return "";
}


