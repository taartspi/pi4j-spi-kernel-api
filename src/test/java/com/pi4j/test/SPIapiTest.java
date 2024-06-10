package com.pi4j.test;

import com.pi4j.Pi4J;
import com.pi4j.context.Context;
import com.pi4j.exception.LifecycleException;
import com.pi4j.library.spiKernelApi.SPIapi.spiApi_t;
import com.pi4j.library.spiKernelApi.SPIapiFuncsNative;
import com.pi4j.library.spiKernelApi.SPIapiImpl;
import com.pi4j.library.spiKernelApi.SPIapiIntrf;
import com.pi4j.library.spiKernelApi.SPIapi;
import com.pi4j.util.Console;
import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import sun.misc.Signal;
import sun.misc.SignalHandler;

public class SPIapiTest {


    public static void main(String[] args) throws Exception {
        var console = new Console();
        Context pi4j = Pi4J.newAutoContext();

        console.title("<-- The Pi4J V2 Project Extension  -->", "Use JNA and the Kernel SPI APIs");



        int pixels = 1;
        int duty = 0;
        int freq = 1;
        int duration = 0;
        boolean doTest = false;

        Signal.handle(new Signal("INT"), new SignalHandler() {
            public void handle(Signal sig) {
                System.out.println("Performing ctl-C shutdown");
                try {
                    pi4j.shutdown();
                } catch (LifecycleException e) {
                    e.printStackTrace();
                }
                // Thread.dumpStack();
                System.exit(2);
            }
        });

        for (int i = 0; i < args.length; i++) {
            String o = args[i];
            if (o.contentEquals("-duty")) {
                String a = args[i + 1];
                duty = Integer.parseInt(a.substring(0));
                i++;
            } else if (o.contentEquals("-freq")) {
                String a = args[i + 1];
                freq = Integer.parseInt(a.substring(0));
                i++;
            } else if (o.contentEquals("-duration")) {
                String a = args[i + 1];
                duration = Integer.parseInt(a.substring(0));
                i++;
            } else if (o.contentEquals("-test")) {
                doTest = true;
            } else {
                console.println("  !!! Invalid Parm " + o);
                console.println("  -duty, -freq,  - duration  -test");
                System.exit(42);
            }
        }

// next line not needed
     ////  SPIapiFuncsNative foo = new SPIapiFuncsNative("libpi4j-SPIapi.so");
        console.println(System.getProperty("user.dir"));
       SPIapiImpl functionsV1 = SPIapiIntrf.createSPIapiImpl();
       final SPIapiFuncsNative functionsV2 = new SPIapiFuncsNative("libpi4j-SPIapi.so");


        SPIapi.spiApi_test dev_test = new SPIapi.spiApi_test();
        dev_test.waldo = 5;
        dev_test.where = 20;

        int returnVal = functionsV1.spiApi_render_test(dev_test);
        console.println("Render_test v1  :  " + dev_test.waldo + "  retVal  : " +returnVal);
        console.println("Render_test v1  :  " + dev_test.where + "  retVal  : " +returnVal);


        spiApi_t spiApi = new spiApi_t();
        spiApi.max_freq = (byte) 127;
    //    SPIapi.spiApi_device_p device = new SPIapi.spiApi_device_p();

        spiApi.dev = new SPIapi.spiApi_device_p.ByReference();

        spiApi.dev.spi_FD = 42;
        spiApi.dev.driver_mode = 5;
        console.println("B4 freq : "  + spiApi.max_freq);

        console.println("B4 spi_FD : "  + spiApi.dev.spi_FD);

        console.println("B4 driver_mode:  "  + spiApi.dev.driver_mode);


        SPIapi.spiApi_return_t ret = functionsV1.spiApi_init( spiApi) ;

        console.println("freq : "  + spiApi.max_freq);

        console.println("spi_FD : "  + spiApi.dev.spi_FD);

        console.println("driver_mode:  "  + spiApi.dev.driver_mode);


        console.println("Render v1 :  " + functionsV1.spiApi_render(spiApi));
    }

}

