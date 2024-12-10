#include <jni.h>
#include <sstream>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_e7_MainActivity_calculate(JNIEnv *env, jobject thiz, jint matrix_size) {
    std::stringstream ss;
    ss << matrix_size << '\n';

    return env->NewStringUTF(ss.str().c_str());
}
