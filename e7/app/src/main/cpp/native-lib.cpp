#include <jni.h>
#include <sstream>
#include <omp.h>

double *a, *b, *c;

void multiply(const int &matrix_size) {
#pragma omp parallel for
    for (int i = 0; i < matrix_size; ++i) {
        for (int k = 0; k < matrix_size; ++k) {
            double a_ik = a[i * matrix_size + k];
            for (int j = 0; j < matrix_size; ++j) {
                c[i * matrix_size + j] += a_ik * b[k * matrix_size + j];
            }
        }
    }
}

std::string measure_time_parallel(const int &matrix_size, const int &cpus) {
    double start = omp_get_wtime();
    omp_set_num_threads(cpus);

    multiply(matrix_size);
    return "Multiplication time: " + std::to_string(omp_get_wtime() - start) + "s\n";
}

void initialize_matrix(const int &matrix_size) {
    a = new double[matrix_size * matrix_size];
    b = new double[matrix_size * matrix_size];
    c = new double[matrix_size * matrix_size];

    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            int index = i * matrix_size + j;

            a[index] = 1;
            b[index] = 1;
            c[index] = 0.f;
        }
    }
}

void deallocate_matrix() {
    delete[] a;
    delete[] b;
    delete[] c;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_e7_MainActivity_calculate(JNIEnv *env, jobject thiz, jint matrix_size, jint cpus) {
    std::string result;

    initialize_matrix(matrix_size);

    result = measure_time_parallel(matrix_size, cpus);

    deallocate_matrix();

    return env->NewStringUTF(result.c_str());
}
