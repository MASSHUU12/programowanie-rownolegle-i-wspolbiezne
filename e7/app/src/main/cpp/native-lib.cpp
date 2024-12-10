#include <jni.h>
#include <string>
#include <iostream>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_e7_MainActivity_calculate(JNIEnv *env, jobject thiz) {
    std::stringstream ss;

    return env->NewStringUTF("Calculate\n");
}
