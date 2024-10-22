#include <chrono>
#include <iostream>
#include <random>
#include <thread>

int cpus{};
int matrix_size{};

double **a, **b, **c, **bt;

void multiply_sequential() {
  for (int i = 0; i < matrix_size; ++i) {
    for (int j = 0; j < matrix_size; ++j) {
      for (int k = 0; k < matrix_size; ++k) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }
}

void multiply_parallel(int start, int end) {
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

void multiply_sequential_transposed() {
  for (int i = 0; i < matrix_size; ++i) {
    for (int j = 0; j < matrix_size; ++j) {
      for (int k = 0; k < matrix_size; ++k) {
        c[i][j] += a[i][k] * bt[j][k];
      }
    }
  }
}

void multiply_parallel_transposed(int start, int end) {
  for (int i = start; i < end; ++i) {
    for (int j = 0; j < matrix_size; ++j) {
      for (int k = 0; k < matrix_size; ++k) {
        c[i][j] += a[i][k] * bt[j][k];
      }
    }
  }
}

void measure_time_parallel(int transpose) {
  std::thread threads[cpus];
  if (transpose == 0) {
    // Parallel multiplication time
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < cpus; ++i) {
      int startRow = i * matrix_size / cpus;
      int endRow = (i + 1) * matrix_size / cpus;
      threads[i] = std::thread(multiply_parallel, startRow, endRow);
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

  // Parallel multiplication time with transposed matrix B
  for (int i = 0; i < matrix_size; ++i) {
    for (int j = 0; j < matrix_size; ++j) {
      c[i][j] = 0.f;
    }
  }
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < cpus; ++i) {
    int startRow = i * matrix_size / cpus;
    int endRow = (i + 1) * matrix_size / cpus;
    threads[i] = std::thread(multiply_parallel_transposed, startRow, endRow);
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

void initialize_matrix() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dis(0.f, 1.f);

  a = new double*[matrix_size];
  b = new double*[matrix_size];
  c = new double*[matrix_size];
  bt = new double*[matrix_size];

  for (int i = 0; i < matrix_size; ++i) {
    a[i] = new double[matrix_size];
    b[i] = new double[matrix_size];
    c[i] = new double[matrix_size];
    bt[i] = new double[matrix_size];

    for (int j = 0; j < matrix_size; ++j) {
      a[i][j] = dis(gen);
      b[i][j] = dis(gen);
      c[i][j] = 0.f;
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

int convert_string_to_int(const std::string &str) {
  try {
    return std::stoi(str);
  } catch (const std::invalid_argument &ex) {
    std::cerr << "Invalid number: " << str << '\n';
    exit(1);
  } catch (const std::out_of_range &ex) {
    std::cerr << "Number out of range: " << str << '\n';
    exit(1);
  }
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: <cpus> <size> <transpose>\n";
    return 1;
  }

  cpus = convert_string_to_int(argv[1]);
  matrix_size = convert_string_to_int(argv[2]);

  if (cpus < 1 || matrix_size < 1) {
    std::cerr << "ERR: Number of CPUs/matrix size less than 1.\n";
    return 1;
  }

  int transpose = convert_string_to_int(argv[3]);
  if (transpose != 0 && transpose != 1) {
    std::cerr << "ERR: Expected value to be 0 or 1.\n";
    return 1;
  }

  initialize_matrix();

  if (cpus == 1) {
    if (transpose == 0) {
      // Time of sequential multiplication
      auto start = std::chrono::high_resolution_clock::now();
      multiply_sequential();
      auto end = std::chrono::high_resolution_clock::now();
      auto time =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
              .count();

      std::cout << "Time of sequential multiplication: " << time << "ms\n";
    } else {
      // B matrix transpose time
      auto start = std::chrono::high_resolution_clock::now();
      transpose_b();
      auto end = std::chrono::high_resolution_clock::now();
      auto time =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
              .count();

      std::cout << "B matrix transpose time: " << time << "ms\n";

      // Sequential multiplication time with matrix transpose B
      for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
          c[i][j] = 0.f;
        }
      }
      start = std::chrono::high_resolution_clock::now();
      multiply_sequential_transposed();
      end = std::chrono::high_resolution_clock::now();
      time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                 .count();

      std::cout << "Sequential multiplication time with matrix transpose B: "
                << time << "ms\n";
    }

    deallocate_matrix();
    return 0;
  }

  measure_time_parallel(transpose);

  deallocate_matrix();
  return 0;
}
