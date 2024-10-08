#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

static const int num_threads = 10;
 
void call_from_thread(int tid, int n) {
    std::cout << "Launched by thread " << tid << std::endl;
    
    int* flatArray_S = new int[n * n];
    int** S = new int*[n];

    for(int i = 0; i < n; i++) {
        S[i] = &flatArray_S[i * n];
    }
}

int main(int argc, char** argv) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::vector<std::thread> th;
 
    for (int i = 0; i < argv[1]; ++i) {
        th.push_back(std::thread(call_from_thread, i, i));
    }
 
    std::cout << "Launched from the main\n";
 
    for (auto &t : th) {
        t.join();
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    
    std::cout << "Time: " << duration << " ns\n"; 
    return 0;
}

