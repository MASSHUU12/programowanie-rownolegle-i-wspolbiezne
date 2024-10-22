#include <chrono>
#include <iostream>
#include <thread>

int cpus{};
int matrix_size{};

double **a, **b, **c, **bt;

void multiply(int start, int end) {
  for (int i = start; i < end; ++i) {
    for (int j = 0; j < matrix_size; ++j) {
      for (int k = 0; k < matrix_size; ++k) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }
}

void transpose_b() {
  for (int i = 0; i < matrix_size; ++i) {
    for (int j = 0; j < matrix_size; ++j) {
      bt[i][j] = b[j][i];
    }
  }
}

void multiply_transposed(int start, int end) {
  for (int i = start; i < end; ++i) {
    for (int j = 0; j < matrix_size; ++j) {
      for (int k = 0; k < matrix_size; ++k) {
        c[i][j] += a[i][k] * bt[j][k];
      }
    }
  }
}

void measure_time_transpose() {
  auto start = std::chrono::high_resolution_clock::now();
  transpose_b();
  auto end = std::chrono::high_resolution_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();

  std::cout << "B matrix transpose time: " << time << "ms\n";
}

void measure_time_parallel(int transpose) {
  std::thread threads[cpus];
  if (transpose == 0) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < cpus; ++i) {
      int startRow = i * matrix_size / cpus;
      int endRow = (i + 1) * matrix_size / cpus;
      threads[i] = std::thread(multiply, startRow, endRow);
    }

    for (int i = 0; i < cpus; ++i) {
      threads[i].join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();

    std::cout << "Parallel multiplication time: " << time << "ms\n";
    return;
  }

  measure_time_transpose();

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < cpus; ++i) {
    int startRow = i * matrix_size / cpus;
    int endRow = (i + 1) * matrix_size / cpus;
    threads[i] = std::thread(multiply_transposed, startRow, endRow);
  }

  for (int i = 0; i < cpus; ++i) {
    threads[i].join();
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();

  std::cout << "Parallel multiplication time with transposed matrix B: " << time
            << "ms\n";
}

void measure_time_sequential(int transpose) {
  if (transpose == 0) {
    auto start = std::chrono::high_resolution_clock::now();
    multiply(0, matrix_size);
    auto end = std::chrono::high_resolution_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();

    std::cout << "Time of sequential multiplication: " << time << "ms\n";
    return;
  }

  measure_time_transpose();

  auto start = std::chrono::high_resolution_clock::now();
  multiply_transposed(0, matrix_size);
  auto end = std::chrono::high_resolution_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();

  std::cout << "Sequential multiplication time with transposed matrix B: "
            << time << "ms\n";
}

void initialize_matrix() {
  a = new double *[matrix_size];
  b = new double *[matrix_size];
  c = new double *[matrix_size];
  bt = new double *[matrix_size];

  for (int i = 0; i < matrix_size; ++i) {
    a[i] = new double[matrix_size];
    b[i] = new double[matrix_size];
    c[i] = new double[matrix_size];
    bt[i] = new double[matrix_size];

    for (int j = 0; j < matrix_size; ++j) {
      a[i][j] = 1;
      b[i][j] = 1;
      c[i][j] = 0.f;
      bt[i][j] = 0.f;
    }
  }
}

void deallocate_matrix() {
  for (int i = 0; i < matrix_size; ++i) {
    delete[] a[i];
    delete[] b[i];
    delete[] c[i];
    delete[] bt[i];
  }

  delete[] a;
  delete[] b;
  delete[] c;
  delete[] bt;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: <cpus> <size> <transpose>\n";
    return 1;
  }

  try {
    cpus = std::stoi(argv[1]);
    matrix_size = std::stoi(argv[2]);

    if (cpus < 1 || matrix_size < 1) {
      std::cerr << "ERR: Number of CPUs/matrix size less than 1.\n";
      return 1;
    }

    int transpose = std::stoi(argv[3]);
    if (transpose != 0 && transpose != 1) {
      std::cerr << "ERR: Expected value to be 0 or 1.\n";
      return 1;
    }

    initialize_matrix();

    if (cpus == 1) {
      measure_time_sequential(transpose);
    } else {
      measure_time_parallel(transpose);
    }
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
