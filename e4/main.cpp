#include <iostream>
#include <omp.h>

int cpus{}, matrix_size{}, block_size = 32;
double *a, *b, *c, *bt;

void multiply() {
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

void transpose_b() {
#pragma omp parallel for
  for (int i = 0; i < matrix_size; ++i) {
    for (int j = 0; j < matrix_size; ++j) {
      bt[i * matrix_size + j] = b[j * matrix_size + i];
    }
  }
}

void multiply_transposed() {
#pragma omp parallel for
  for (int i = 0; i < matrix_size; ++i) {
    for (int j = 0; j < matrix_size; ++j) {
      double sum = 0.0;
      for (int k = 0; k < matrix_size; ++k) {
        sum += a[i * matrix_size + k] * bt[j * matrix_size + k];
      }
      c[i * matrix_size + j] = sum;
    }
  }
}

void multiply_tiled() {
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

void measure_time_transpose() {
  auto start = omp_get_wtime();
  transpose_b();

  std::cout << "B matrix transpose time: " << omp_get_wtime() - start << "s\n";
}

void measure_time_parallel(int method) {
  auto start = omp_get_wtime();
  omp_set_num_threads(cpus);

  if (method == 0) {
    multiply();
    std::cout << "Parallel multiplication time: " << omp_get_wtime() - start
              << "s\n";
  } else if (method == 1) {
    measure_time_transpose();
    multiply_transposed();
    std::cout << "Parallel multiplication time with transposed matrix B: "
              << omp_get_wtime() - start << "s\n";
  } else if (method == 2) {
    multiply_tiled();
    std::cout << "Parallel multiplication time with tiling: "
              << omp_get_wtime() - start << "s\n";
  }
}

void measure_time_sequential(int method) {
  auto start = omp_get_wtime();
  omp_set_num_threads(1);

  if (method == 0) {
    multiply();
    std::cout << "Time of sequential multiplication: "
              << omp_get_wtime() - start << "s\n";
  } else if (method == 1) {
    measure_time_transpose();
    multiply_transposed();
    std::cout << "Sequential multiplication time with transposed matrix B: "
              << omp_get_wtime() - start << "s\n";
  } else if (method == 2) {
    multiply_tiled();
    std::cout << "Sequential multiplication time with tiling: "
              << omp_get_wtime() - start << "s\n";
  }
}

void initialize_matrix() {
  a = new double[matrix_size * matrix_size];
  b = new double[matrix_size * matrix_size];
  c = new double[matrix_size * matrix_size];
  bt = new double[matrix_size * matrix_size];

  for (int i = 0; i < matrix_size; ++i) {
    for (int j = 0; j < matrix_size; ++j) {
      int index = i * matrix_size + j;

      a[index] = 1;
      b[index] = 1;
      c[index] = 0.f;
      bt[index] = 0.f;
    }
  }
}

void deallocate_matrix() {
  delete[] a;
  delete[] b;
  delete[] c;
  delete[] bt;
}

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cerr << "Usage: <cpus> <size> <method> [block_size]\n";
    return 1;
  }

  try {
    cpus = std::stoi(argv[1]);
    matrix_size = std::stoi(argv[2]);

    if (cpus < 1 || matrix_size < 1) {
      std::cerr << "ERR: Number of CPUs/matrix size less than 1.\n";
      return 1;
    }

    int method = std::stoi(argv[3]);
    if (method < 0 || method > 2) {
      std::cerr << "ERR: Expected method to be 0, 1, or 2.\n";
      return 1;
    }

    if (method == 2 && argc >= 5) {
      block_size = std::stoi(argv[4]);
      if (block_size < 1 || block_size > matrix_size) {
        std::cerr << "ERR: Invalid block size.\n";
        return 1;
      }
    }

    initialize_matrix();

    cpus == 1 ? measure_time_sequential(method) : measure_time_parallel(method);
  } catch (const std::invalid_argument &e) {
    std::cerr << "ERR: Invalid argument.\n";
    return 1;
  } catch (const std::out_of_range &e) {
    std::cerr << "ERR: Out of range.\n";
    return 1;
  }

  deallocate_matrix();
  return 0;
}
