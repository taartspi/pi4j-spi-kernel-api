package com.pi4j.library.spiKernelApi;


import com.pi4j.Pi4J;
import com.pi4j.context.Context;
import com.pi4j.util.Console;
import com.sun.jna.*;
import com.sun.jna.Structure.*;


public class SPIapi {

    Console console = new Console();
    Context pi4j =  Pi4J.newAutoContext();




    public final int RPI_HWVER_TYPE_UNKNOWN = 0;
    public final int RPI_HWVER_TYPE_PI1 = 1;
    public final int RPI_HWVER_TYPE_PI2 = 2;
    public final int RPI_HWVER_TYPE_PI4 = 3;

    public final int RPI_PWM_CHANNELS = 2;

    @FieldOrder({"returnEnumGood","returnEnumFail"})
    public static class spiApi_return_t extends Structure {
        public int returnEnumGood = 0;
        public int returnEnumFail = -1;
    }

    @FieldOrder({"waldo","where"})
    public static class spiApi_test extends Structure {
        public int waldo;
        public int where;
    }

    /*typedef struct spiApi_t
{
    uint64_t render_wait_time;                   //< time in µs before the next render can run
    struct spiApi_device *device;                //< Private data for driver use
    const rpi_hw_t *rpi_hw;                      //< RPI Hardware Information
    uint32_t freq;                               //< Required output frequency
    int dmanum;                                  //< DMA number _not_ already in use
    spiApi_channel_t channel[RPI_PWM_CHANNELS];
} spiApi_t;(/


public static class Example10Struct extends Structure {
		public static class ByReference extends Example10Struct implements Structure.ByReference {}

		public int numVals;
		public Pointer vals; // double*
	}
	...
 */
    @FieldOrder({"max_freq", "dev"})
    public static class spiApi_t extends Structure{
         public byte max_freq;
            public spiApi_device_p.ByReference dev;
        public spiApi_t() {
            super();
        }
    }

    @FieldOrder({"driver_mode","spi_FD"})
    public static class spiApi_device_p extends  Structure {
             public int driver_mode;
             public int spi_FD;

        public spiApi_device_p(Pointer peer) {
            super(peer);
        }
        public spiApi_device_p() {
            super();
        }
//ALIGN_NONE
        public static class ByReference extends spiApi_device_p implements Structure.ByReference {

        };

        public static class ByValue extends spiApi_device_p implements Structure.ByValue {
        };
    }


    /*
    typedef struct {
    uint32_t type;
#define RPI_HWVER_TYPE_UNKNOWN                   0
#define RPI_HWVER_TYPE_PI1                       1
#define RPI_HWVER_TYPE_PI2                       2
#define RPI_HWVER_TYPE_PI4                       3
    uint32_t hwver;
    uint32_t periph_base;
    uint32_t videocore_base;
    char *desc;
} rpi_hw_t;
     */
 /*   @FieldOrder({"type","hwver","periph_base","videocore_base","desc"})
    public static class rpi_hw_t extends Structure {
        public int type;
        public int hwver;
        public int periph_base;
        public int videocore_base;
        public String desc;
    };
*/



    /*
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
    uint8_t *gamma;                                 //< Gamma correction table
} spiApi_channel_t;
     */
    @FieldOrder({"gpionum","invert","count","strip_type",
        "leds","brightness","wshift","rshift","gshift","bshift","gamma"})
    public static class spiApi_channel_t extends Structure {

        public int gpionum;                                 //< GPIO Pin with PWM alternate function, 0 if unused
        public int invert;                                  //< Invert output signal
        public int count;                                   //< Number of LEDs, 0 if channel is unused
        public int strip_type;                              //< Strip color layout -- one of spiApi_STRIP_xxx constants
        public int leds;                          //  TODO *   spiApi_led_t  typed to unot32 < LED buffers, allocated by driver based on count
        public byte brightness;                          //< Brightness value between 0 and 255
        public byte wshift;                              //< White shift value
        public byte rshift;                              //< Red shift value
        public byte gshift;                              //< Green shift value
        public byte bshift;                              //< Blue shift value
        public byte gamma;      // TODO  *

    }
/*typedef struct spiApi_device
{
    int driver_mode;
    volatile uint8_t *pxl_raw;
    volatile dma_t *dma;
    volatile pwm_t *pwm;
    volatile pcm_t *pcm;
    int spi_fd;
    volatile dma_cb_t *dma_cb;
    uint32_t dma_cb_addr;
    volatile gpio_t *gpio;
    volatile cm_clk_t *cm_clk;
    videocore_mbox_t mbox;
    int max_count;
} spiApi_device_t; */


    // pointer
    @FieldOrder({"spi_mode","spi_FD"})
    public static class spiApi_device_p_old extends Structure{
        public int spi_mode;
        public int spi_FD;
      //  public Pointer p;


          /*  public spiApi_device_p() {
                super();
                // handle fixed-size array fields correctly
                ensureAllocated();
            }*/
    }





    }
