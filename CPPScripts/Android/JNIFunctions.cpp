#include <jni.h>
#include <dlfcn.h>

#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>
extern "C"
{
#include <game-activity/native_app_glue/android_native_app_glue.c>
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_google_androidgamesdk_GameActivity_getDlError(
    JNIEnv* env,
    jobject /* this */)
{
    const char* error = dlerror();
    return env->NewStringUTF(error ? error : "No dlerror");
}

extern "C" JNIEXPORT jint
JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return JNI_ERR;
    }

    // 进行一些初始化操作，例如注册本地方法

    return JNI_VERSION_1_6;
}