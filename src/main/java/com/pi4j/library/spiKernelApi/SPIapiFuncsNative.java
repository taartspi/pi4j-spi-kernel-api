package com.pi4j.library.spiKernelApi;


//import org.apache.commons.io.F


public class SPIapiFuncsNative implements SPIapiIntrf{
    private final SPIapiIntrf functionsNative = null;
    static {
        // Load the platform library
       ;// NativeLibraryLoader.load("libpi4j-SPIapi.so", "pi4j-SPIapi");
    }

    static {
        // Load the platform library
        ;//NativeLibraryLoader.load("libpi4j-SPIapi.so", "libpi4j-SPIapi");
    }

    public static SPIapiImpl newNativeInstance(String soName, String libName) throws java.io.IOException {
        SPIapiImpl rval = new SPIapiImpl(soName);
        rval.initialize(soName, libName);
        return(rval);
    }


    public SPIapiFuncsNative(String fileName) throws  java.io.IOException {
     }

    public void initialize(String fileName) throws  java.io.IOException {
       // todo this.functionsNative = Native.loadLibrary(extractFile(fileName), SPIapiIntrf.class);
    }


    @Override
    public int spiApi_render_test(SPIapi.spiApi_test spiApi) {
        return jna_rc.x0;
    }

    public SPIapi.spiApi_return_t spiApi_render(final SPIapi.spiApi_t spiApi) {
        return functionsNative.spiApi_render(spiApi);
    }







}
