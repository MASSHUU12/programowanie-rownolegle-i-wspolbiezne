#include <jni.h>
#include <sstream>
#include <omp.h>

double *a, *b, *c;

void multiply_tiled(const int &matrix_size, const int &block_size) {
#pragma omp parallel for collapse(2)
    for (int ii = 0; ii < matrix_size; ii += block_size) {
        for (int jj = 0; jj < matrix_size; jj += block_size) {
            for (int kk = 0; kk < matrix_size; kk += block_size) {
                int i_max = std::min(ii + block_size, matrix_size);
                int j_max = std::min(jj + block_size, matrix_size);
                int k_max = std::min(kk + block_size, matrix_size);
                for (int i = ii; i < i_max; ++i) {
                    for (int k = kk; k < k_max; ++k) {
                        double a_ik = a[i * matrix_size + k];
                        for (int j = jj; j < j_max; ++j) {
                            c[i * matrix_size + j] += a_ik * b[k * matrix_size + j];
                        }
                    }
                }
            }
        }
    }
}

std::string measure_time_parallel(const int &matrix_size, const int &block_size) {
    double start = omp_get_wtime();
//    omp_set_num_threads(cpus); // TODO

    multiply_tiled(matrix_size, block_size);
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
Java_com_example_e7_MainActivity_calculate(JNIEnv *env, jobject thiz, jint matrix_size, jint block_size) {
    std::stringstream ss;

    initialize_matrix(matrix_size);

    ss << "Matrix size: " << matrix_size << '\n'
        << "Block size: " << block_size << '\n'
        << "CPUs: " << "TODO" << "\n\n"
        << measure_time_parallel(matrix_size, block_size);

    deallocate_matrix();

    return env->NewStringUTF(ss.str().c_str());
}
