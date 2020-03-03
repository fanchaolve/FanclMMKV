package com.fancl.fanclmmkv;

import android.os.Bundle;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class MMKVActvity extends AppCompatActivity {

    private static final String TAG = "MMKVActvity";

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        FanclMMKV.initialize(getFilesDir().getAbsolutePath() + "/mmkv");
        FanclMMKV mmkv = FanclMMKV.defaultMMKV();

        Log.e(TAG, "a=" + mmkv.getInt("a", 0));
        Log.e(TAG, "b=" + mmkv.getInt("b", 0));
        Log.e(TAG, "c=" + mmkv.getInt("c", 0));
        Log.e(TAG, "d=" + mmkv.getInt("d", 0));


        mmkv.putInt("a", 1);
        mmkv.putInt("b", -1);
        mmkv.putInt("c", 128);
        mmkv.putInt("d", -128);

        Log.e(TAG, "a=" + mmkv.getInt("a", 0));
        Log.e(TAG, "b=" + mmkv.getInt("b", 0));
        Log.e(TAG, "c=" + mmkv.getInt("c", 0));
        Log.e(TAG, "d=" + mmkv.getInt("d", 0));
    }
}
