package com.fancl.fanclmmkv;

import android.content.Context;

public class FanclMMKV {

    static {
        System.loadLibrary("mmkv");
    }

    private static String rootDir;
    private final long nativeHandle;

    public FanclMMKV(long handle) {
        nativeHandle = handle;
    }

    //初始化mmkv
    public static String initialize(Context context) {
        String rootDir = context.getFilesDir().getAbsolutePath() + "/mmkv";
        return initialize(rootDir);
    }

    public static String initialize(String rootDir) {
        FanclMMKV.rootDir = rootDir;
        jniInitialize(FanclMMKV.rootDir);
        return rootDir;
    }


    public static FanclMMKV defaultMMKV() {
        if (rootDir == null) {
            throw new IllegalStateException("You should Call MMKV.initialize() first.");
        }
        //返回 mmkv对象的句柄
        long handle = getDefaultMMKV();
        return new FanclMMKV(handle);
    }




    public int getInt(String key, int defaultValue) {
        return getInt(nativeHandle, key, defaultValue);
    }

    public void putInt(String key, int value) {
        putInt(nativeHandle, key, value);
    }

    private static native void jniInitialize(String rootDir);

    private static native long getDefaultMMKV();

    private static native int getInt(long nativeHandle, String key, int defaultValue);

    private static native void putInt(long nativeHandle, String key, int value);

}
