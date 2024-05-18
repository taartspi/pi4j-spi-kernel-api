package com.pi4j.library.spiKernelApi;

import com.pi4j.library.spiKernelApi.util.NativeLibraryLoader;
import com.pi4j.library.spiKernelApi.util.StringUtil;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import com.sun.jna.Native;
import com.sun.jna.NativeLibrary;
import java.nio.file.Path;
import java.nio.file.Paths;


public class SPIapiImpl  implements SPIapiIntrf {

    SPIapiIntrf functionsNative = null;


    public int x = 42;
    public SPIapiImpl(String fileName) throws  java.io.IOException {
    }

    public SPIapiImpl() {
    }


    public static SPIapiImpl newInstance(String fileName, String libName) throws IOException {
        SPIapiImpl impl = new SPIapiImpl();
        impl.initialize(fileName, libName);
        return(impl);
    }

    @Override
    //SPIapi.spiApi_return_t
    public int spiApi_render_test(SPIapi.spiApi_test spiApi) {
        int ret = jna_rc.x0;
        ret = this.functionsNative.spiApi_render_test(spiApi);

        return ret;
    }

    @Override
    public SPIapi.spiApi_return_t spiApi_render(SPIapi.spiApi_t spiApi) {
        return null;
    }


    public void initialize(String fileName, String libName) throws java.io.IOException {
        // the following allows my usage of the JNA code, rather than cloning my own version
        Path workingDirectory=Paths.get(".").toAbsolutePath();
        // get CPU architecture from system properties
        String osArch = System.getProperty("os.arch").toLowerCase();

        // sanitize CPU architecture string
        switch (osArch) {
            case "arm":
                osArch = "armhf";
                break;
            case "arm64":
                osArch = "aarch64";
                break;
            case "aarch64":
                break;
            default:
                throw new IllegalStateException("Pi4J has detected and UNKNOWN/UNSUPPORTED 'os.arch' : [" +
                    osArch + "]; only 'arm|armhf' and 'arm64|aarch64' are supported.");
        }
        String possibleJnaPath = workingDirectory+"/target/lib/" +osArch + "/pi4j-SPIapi/";
        // Also, handle the user relying upon "pi4j.library.path" to locate so's
        String pi4jLibpath = System.getProperty("pi4j.library.path");

        if(StringUtil.isNotNullOrEmpty(pi4jLibpath, true)){
            possibleJnaPath += ":"+pi4jLibpath;
        }
        System.setProperty("jna.library.path", possibleJnaPath);
        NativeLibraryLoader.load(fileName, libName);
        NativeLibrary lib = NativeLibrary.getInstance(fileName); //SPIapiIntrf.getCLibName(LIBPROPFILE));
        SPIapiIntrf INSTANCE = Native.load( SPIapiIntrf.class);
        this.functionsNative = INSTANCE; //Native.loadLibrary(extractFile(fileName), String.valueOf(SPIapiIntrf.class));
    }

    private String extractFile(final String fileName) throws com.pi4j.io.exception.IOException, java.io.IOException {
        final InputStream source = SPIapiFuncsNative.class.getClassLoader().getResourceAsStream(fileName);
        final File file = File.createTempFile("lib", null);
        copyInputStreamToFile(source, file);
        return file.getAbsolutePath();
    }

    static final int DEFAULT_BUFFER_SIZE = 8192;

    private static void copyInputStreamToFile(InputStream inputStream, File file)
        throws com.pi4j.io.exception.IOException, java.io.IOException {
        // append = false
        try (FileOutputStream outputStream = new FileOutputStream(file, false)) {
            int read;
            byte[] bytes = new byte[DEFAULT_BUFFER_SIZE];
            while ((read = inputStream.read(bytes)) != -1) {
                outputStream.write(bytes, 0, read);
            }
        }

    }

}