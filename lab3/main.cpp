#include <cmath>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

// Screen coordinate
int IX_MAX{};
int IY_MAX{};

// World coordinate = parameter plane
constexpr double CX_MIN = -2.5;
constexpr double CX_MAX = 1.5;
constexpr double CY_MIN = -2.0;
constexpr double CY_MAX = 2.0;

double PIXEL_WIDTH = (CX_MAX - CX_MIN) / IX_MAX;
double PIXEL_HEIGHT = (CY_MAX - CY_MIN) / IY_MAX;

const std::string FILENAME = "new1.ppm";
const std::string COMMENT = "# ";

// Iteration and bail-out value
const int ITERATION_MAX = 200;
constexpr double ESCAPE_RADIUS = 2;
constexpr double ER2 = ESCAPE_RADIUS * ESCAPE_RADIUS;

unsigned char *IMAGE_DATA;

constexpr int NUM_THREADS = 4;
int CHUNK_SIZE{};
std::thread threads[NUM_THREADS];

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

void computeMandelbrot(int thread_num, int start, int end) {
  for (int iY = start; iY < end; iY++) {
    for (int iX = 0; iX < IX_MAX; iX++) {
      double Cx = CX_MIN + iX * PIXEL_WIDTH;
      double Cy = CY_MIN + iY * PIXEL_HEIGHT;

      double Zx = 0.0;
      double Zy = 0.0;
      double Zx2 = Zx * Zx;
      double Zy2 = Zy * Zy;

      int Iteration = 0;
      while (Iteration < ITERATION_MAX &&
             (Zx2 + Zy2) < ER2) {
        Zy = 2 * Zx * Zy + Cy;
        Zx = Zx2 - Zy2 + Cx;
        Zx2 = Zx * Zx;
        Zy2 = Zy * Zy;
        Iteration++;
      }

      if (Iteration == ITERATION_MAX) {
        IMAGE_DATA[iY * IX_MAX * 3 + iX * 3] = 0;
        IMAGE_DATA[iY * IX_MAX * 3 + iX * 3 + 1] = 0;
        IMAGE_DATA[iY * IX_MAX * 3 + iX * 3 + 2] = 0;
      } else {
        // Prevent black color in the first thread
        int base = thread_num + 1;

        // Fancy colors
        int r = (base * 50) % 256;
        int g = (base * 100) % 256;
        int b = (base * 150) % 256;

        IMAGE_DATA[iY * IX_MAX * 3 + iX * 3] = r;
        IMAGE_DATA[iY * IX_MAX * 3 + iX * 3 + 1] = g;
        IMAGE_DATA[iY * IX_MAX * 3 + iX * 3 + 2] = b;
      }
    }
  }
}

int main(int argc, char** argv) {
  if (argc < 2) { // TODO: Handle number of threads
    std::cerr << "Program requires resolution and number of threads to be passed.\n";
    return 1;
  }

  //int res = std::stoi(argv[1]);

  int res = convert_string_to_int(argv[1]);
  IX_MAX = res;
  IY_MAX = res;
  CHUNK_SIZE = IY_MAX / NUM_THREADS;

  IMAGE_DATA = new unsigned char[IX_MAX * IY_MAX * 3];

  //int ths = convert_string_to_int(argv[2]);
  //NUM_THREADS = ths;

  std::cout << "Image " << FILENAME << " (" << IX_MAX << 'x' << IY_MAX << ")\n";
  //std::cout << "Threads: " << NUM_THREADS << '\n';

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NUM_THREADS; i++) {
    int start = i * CHUNK_SIZE;
    int end = (i == NUM_THREADS - 1) ? IY_MAX : (i + 1) * CHUNK_SIZE;
    threads[i] = std::thread(computeMandelbrot, i, start, end);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    threads[i].join();
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "Time: " << time << "ms\n";

  std::ofstream fp(FILENAME, std::ios::binary);
  fp << "P6\n " << COMMENT << "\n " << IX_MAX << "\n " << IY_MAX << "\n "
     << 255 << '\n';
  fp.write((char *)IMAGE_DATA, IX_MAX * IY_MAX * 3);
  fp.close();

  delete[] IMAGE_DATA;

  return 0;
}
