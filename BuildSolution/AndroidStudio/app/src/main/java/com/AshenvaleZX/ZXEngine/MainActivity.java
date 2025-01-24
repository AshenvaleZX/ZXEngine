package com.AshenvaleZX.ZXEngine;

import com.google.androidgamesdk.GameActivity;

public class MainActivity extends GameActivity {
    static {
        System.loadLibrary(BuildConfig.DEBUG ? "assimpd" : "assimp");
        System.loadLibrary(BuildConfig.DEBUG ? "freetyped" : "freetype");
        System.loadLibrary(BuildConfig.DEBUG ? "ZXEngined" : "ZXEngine");
    }
}