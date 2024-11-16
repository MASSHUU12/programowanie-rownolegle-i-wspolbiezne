#include <chrono>
#include <iostream>
#include <omp.h>

void call_from_thread(int tid, int n) {
  std::cout << "Launched by thread " << tid << '\n';

  int *flatArray_S = new int[n * n];
  int **S = new int *[n];

  for (int i = 0; i < n; i++) {
    S[i] = &flatArray_S[i * n];
  }

  delete[] S;
  delete[] flatArray_S;
}

int convert_string_to_int(const std::string& str) {
  try {
    return std::stoi(str);
  } catch (const std::invalid_argument& ex) {
    std::cerr << "Invalid number: " << str << '\n';
    exit(1);
  } catch (const std::out_of_range& ex) {
    std::cerr << "Number out of range: " << str << '\n';
    exit(1);
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Program requires number of threads to be passed\n";
    return 1;
  }

  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();

  int num_threads = convert_string_to_int(argv[1]);
  if (num_threads <= 0) {
    std::cerr << "Number of threads must be greater than 0\n";
    return 1;
  }

  omp_set_num_threads(num_threads);

  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    int n = tid;
    call_from_thread(tid, n);
  }

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

  std::cout << "Time: " << duration << " ns\n";
  return 0;
}
