package com.fancl.fanclmmkv

import android.content.Context



class MMKV(var handle: Long) {


//    private var nativeHandle: Long? = null;
//
//    companion object {
//        private lateinit var rootDir: String
//
//        init {
//            System.loadLibrary("mmkv");
//        }
//
//        fun initialize(context: Context): String {
//            val rootDir = context.filesDir.absolutePath + "/mmkv"
//            return initialize(rootDir)
//        }
//
//        fun initialize(rootDir: String): String {
//            MMKV.rootDir = rootDir
//            jniInitialize(MMKV.rootDir)
//            return rootDir
//        }
//
//        fun defaultMMKV(): MMKV {
//            rootDir?.let {
//                val handle = getDefaultMMKV()
//                return MMKV(handle)
//            }
//        }
//
//
//        private external fun jniInitialize(rootDir: String)
//
//        private external fun getDefaultMMKV(): Long
//        private external fun getInt(
//            nativeHandle: Long,
//            key: String, defaultValue: Int
//        )
//
//        private external fun putInt(
//            nativeHandle: Long,
//            key: String, value: Int
//        )
//    }
//
//
//    init {
//        nativeHandle = handle
//    }

}