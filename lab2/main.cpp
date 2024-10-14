#include <chrono>
#include <iostream>
#include <random>
#include <thread>

const int N = 2048;
const int THREADS_MAX = 16;

double a[N][N], b[N][N], c[N][N], bt[N][N];

void multiply_sequential() {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = 0; k < N; ++k) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }
}

void multiply_parallel(int start, int end) {
  for (int i = start; i < end; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = 0; k < N; ++k) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }
}

void transpose_b() {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      bt[i][j] = b[j][i];
    }
  }
}

void multiply_sequential_transposed() {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = 0; k < N; ++k) {
        c[i][j] += a[i][k] * bt[j][k];
      }
    }
  }
}

void multiply_parallel_transposed(int start, int end) {
  for (int i = start; i < end; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = 0; k < N; ++k) {
        c[i][j] += a[i][k] * bt[j][k];
      }
    }
  }
}

void measure_time_parallel(int thread_num) {
  // Mnozenie macierzy rownolegle
  std::thread threads[thread_num];
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < thread_num; ++i) {
    int startRow = i * N / thread_num;
    int endRow = (i + 1) * N / thread_num;
    threads[i] = std::thread(multiply_parallel, startRow, endRow);
  }

  for (int i = 0; i < thread_num; ++i) {
    threads[i].join();
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();

  std::cout << "Czas mnozenia rownoleglego: " << time << "ms\n";

  // Mnozenie macierzy rownolegle z transponowana macierza B
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      c[i][j] = 0.f;
    }
  }
  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < thread_num; ++i) {
    int startRow = i * N / thread_num;
    int endRow = (i + 1) * N / thread_num;
    threads[i] = std::thread(multiply_parallel_transposed, startRow, endRow);
  }

  for (int i = 0; i < thread_num; ++i) {
    threads[i].join();
  }
  end = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
             .count();

  std::cout << "Czas mnozenia rownoleglego z transponowana macierzy B: " << time
            << "ms\n";
}

void initialize_matrix() {
  // Inicjalizacja macierzy
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dis(0.f, 1.f);

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      a[i][j] = dis(gen);
      b[i][j] = dis(gen);
      c[i][j] = 0.f;
    }
  }
}

int main() {
  initialize_matrix();

  std::cout << "Rozmiar macierzy: " << N << '\n';

  // Mnozenie macierzy sekwencyjnie
  auto start = std::chrono::high_resolution_clock::now();
  multiply_sequential();
  auto end = std::chrono::high_resolution_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();

  std::cout << "Czas mnozenia sekwencyjnego: " << time << "ms\n";

  // Transponowanie macierzy
  start = std::chrono::high_resolution_clock::now();
  transpose_b();
  end = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
             .count();

  std::cout << "Czas transponowania macierzy B: " << time << "ms\n";

  // Mnozenie macierzy sekwencyjnie z transponowaniem macierzy B
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      c[i][j] = 0.f;
    }
  }
  start = std::chrono::high_resolution_clock::now();
  multiply_sequential_transposed();
  end = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
             .count();

  std::cout << "Czas mnozenia sekwencyjnego z transponowaniem macierzy B: "
            << time << "ms\n";

  for (int i = 1; i <= THREADS_MAX; i *= 2) {
    std::cout << "Licza watkow: " << i << '\n';
    measure_time_parallel(i);
  }

  return 0;
}
