#include <iostream>
#include <thread>
#include <chrono>
#include <random>

const int N = 512;
const int THREADS = 4;

double a[N][N], b[N][N], c[N][N], bt[N][N];

void multiplySequential() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < N; ++k) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void multiplyParallel(int start, int end) {
    for (int i = start; i < end; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < N; ++k) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void transposeB() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            bt[i][j] = b[j][i];
        }
    }
}

void multiplySequentialTransposed() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < N; ++k) {
                c[i][j] += a[i][k] * bt[j][k];
            }
        }
    }
}

void multiplyParallelTransposed(int start, int end) {
    for (int i = start; i < end; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < N; ++k) {
                c[i][j] += a[i][k] * bt[j][k];
            }
        }
    }
}

int main() {
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

    // Mnozenie macierzy sekwencyjnie
    auto start = std::chrono::high_resolution_clock::now();
    multiplySequential();
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Czas mnozenia sekwencyjnego: " << time << "ms\n";

    // Mnozenie macierzy rownolegle
    std::thread threads[THREADS];
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < THREADS; ++i) {
        int startRow = i * N / THREADS;
        int endRow = (i + 1) * N / THREADS;
        threads[i] = std::thread(multiplyParallel, startRow, endRow);
    }

    for (int i = 0; i < THREADS; ++i) {
        threads[i].join();
    }
    end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Czas mnozenia rownoleglego: " << time << "ms\n";

    // Transponowanie macierzy
    start = std::chrono::high_resolution_clock::now();
    transposeB();
    end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Czas transponowania macierzy B: " << time << "ms\n";

    // Mnozenie macierzy sekwencyjnie z transponowana macierza B
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            c[i][j] = 0.f;
        }
    }
    start = std::chrono::high_resolution_clock::now();
    multiplySequentialTransposed();
    end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds(end - start).count();

    std::cout << "Czas mnozenia sekwencyjnego z transponowaniem macierzy B: " << time << "ms\n";

    // Mnozenie macierzy rownolegle z transponowana macierza B
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            c[i][j] = 0.f;
        }
    }
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < THREADS; ++i) {
        int startRow = i * N / THREADS;
        int endRow = (i + 1) * N / THREADS;
        threads[i] = std::thread(multiplyParallelTransposed, startRow, endRow);
    }

    for (int i = 0; i < THREADS; ++i) {
        threads[i].join();
    }
    end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Czas mnozenia rownoleglego z transponowana macierzy B: " << time << "ms\n";

    return 0;
}
