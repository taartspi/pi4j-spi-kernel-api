/*
 * spiApi.h
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


#ifndef __spiApi_H__
#define __spiApi_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "rpihw.h"
#include "pwm.h"


#define spiApi_TARGET_FREQ                       800000   // Can go as low as 400000

// 4 color R, G, B and W ordering
#define SK6812_STRIP_RGBW                        0x18100800
#define SK6812_STRIP_RBGW                        0x18100008
#define SK6812_STRIP_GRBW                        0x18081000
#define SK6812_STRIP_GBRW                        0x18080010
#define SK6812_STRIP_BRGW                        0x18001008
#define SK6812_STRIP_BGRW                        0x18000810
#define SK6812_SHIFT_WMASK                       0xf0000000

// 3 color R, G and B ordering
#define spiApi_STRIP_RGB                         0x00100800
#define spiApi_STRIP_RBG                         0x00100008
#define spiApi_STRIP_GRB                         0x00081000
#define spiApi_STRIP_GBR                         0x00080010
#define spiApi_STRIP_BRG                         0x00001008
#define spiApi_STRIP_BGR                         0x00000810

// predefined fixed LED types
#define WS2812_STRIP                             spiApi_STRIP_GRB
#define SK6812_STRIP                             spiApi_STRIP_GRB
#define SK6812W_STRIP                            SK6812_STRIP_GRBW

struct spiApi_device;



 typedef struct spiApi_test  {
        long waldo;
    }spiApi_test_t;



typedef uint32_t spiApi_led_t;                   //< 0xWWRRGGBB
typedef struct spiApi_channel_t
{
    int gpionum;                                 //< GPIO Pin with PWM alternate function, 0 if unused
    int invert;                                  //< Invert output signal
    int count;                                   //< Number of LEDs, 0 if channel is unused
    int strip_type;                              //< Strip color layout -- one of spiApi_STRIP_xxx constants
    spiApi_led_t *leds;                          //< LED buffers, allocated by driver based on count
    uint8_t brightness;                          //< Brightness value between 0 and 255
    uint8_t wshift;                              //< White shift value
    uint8_t rshift;                              //< Red shift value
    uint8_t gshift;                              //< Green shift value
    uint8_t bshift;                              //< Blue shift value
    uint8_t *gamma;                              //< Gamma correction table
} spiApi_channel_t;

typedef struct spiApi_t
{
    uint64_t render_wait_time;                   //< time in µs before the next render can run
    struct spiApi_device *device;                //< Private data for driver use
    const rpi_hw_t *rpi_hw;                      //< RPI Hardware Information
    uint32_t freq;                               //< Required output frequency
    int dmanum;                                  //< DMA number _not_ already in use
    spiApi_channel_t channel[RPI_PWM_CHANNELS];
} spiApi_t;

#define spiApi_RETURN_STATES(X)                                                             \
            X(0, spiApi_SUCCESS, "Success"),                                                \
            X(-1, spiApi_ERROR_GENERIC, "Generic failure"),                                 \
            X(-2, spiApi_ERROR_OUT_OF_MEMORY, "Out of memory"),                             \
            X(-3, spiApi_ERROR_HW_NOT_SUPPORTED, "Hardware revision is not supported"),     \
            X(-4, spiApi_ERROR_MEM_LOCK, "Memory lock failed"),                             \
            X(-5, spiApi_ERROR_MMAP, "mmap() failed"),                                      \
            X(-6, spiApi_ERROR_MAP_REGISTERS, "Unable to map registers into userspace"),    \
            X(-7, spiApi_ERROR_GPIO_INIT, "Unable to initialize GPIO"),                     \
            X(-8, spiApi_ERROR_PWM_SETUP, "Unable to initialize PWM"),                      \
            X(-9, spiApi_ERROR_MAILBOX_DEVICE, "Failed to create mailbox device"),          \
            X(-10, spiApi_ERROR_DMA, "DMA error"),                                          \
            X(-11, spiApi_ERROR_ILLEGAL_GPIO, "Selected GPIO not possible"),                \
            X(-12, spiApi_ERROR_PCM_SETUP, "Unable to initialize PCM"),                     \
            X(-13, spiApi_ERROR_SPI_SETUP, "Unable to initialize SPI"),                     \
            X(-14, spiApi_ERROR_SPI_TRANSFER, "SPI transfer error")                         \

#define spiApi_RETURN_STATES_ENUM(state, name, str) name = state
#define spiApi_RETURN_STATES_STRING(state, name, str) str

typedef enum {
    spiApi_RETURN_STATES(spiApi_RETURN_STATES_ENUM),

    spiApi_RETURN_STATE_COUNT
} spiApi_return_t;


int spiApi_sum(int n1, int n2);

                                            //< Tear it all down
spiApi_return_t spiApi_render_test(spiApi_test_t *spiApitest);



spiApi_return_t spiApi_init(spiApi_t *spiApi);                                  //< Initialize buffers/hardware
void spiApi_fini(spiApi_t *spiApi);                                             //< Tear it all down
spiApi_return_t spiApi_render(spiApi_t *spiApi);                                //< Send LEDs off to hardware
spiApi_return_t spiApi_wait(spiApi_t *spiApi);                                  //< Wait for DMA completion
const char * spiApi_get_return_t_str(const spiApi_return_t state);              //< Get string representation of the given return state
void spiApi_set_custom_gamma_factor(spiApi_t *spiApi, double gamma_factor);     //< Set a custom Gamma correction array based on a gamma correction factor

#ifdef __cplusplus
}
#endif

#endif /* __spiApi_H__ */
