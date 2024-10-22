#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <thread>

int ix_max{}, iy_max{};
double pixel_width{}, pixel_height{};

const double CX_MIN = -2.5;
const double CX_MAX = 1.5;
const double CY_MIN = -2.0;
const double CY_MAX = 2.0;

const int ITERATION_MAX = 200;
constexpr double ESCAPE_RADIUS = 2;
constexpr double ER2 = ESCAPE_RADIUS * ESCAPE_RADIUS;

constexpr int COLOR_BASE = 50;
constexpr int COLOR_R = COLOR_BASE;
constexpr int COLOR_G = COLOR_BASE * 2;
constexpr int COLOR_B = COLOR_BASE * 3;

const std::string FILENAME = "new1.ppm";

unsigned char *image_data;

int cpus{}, chunk_size{};

void computeMandelbrot(int thread_num, int start, int end) {
  for (int iY = start; iY < end; iY++) {
    for (int iX = 0; iX < ix_max; iX++) {
      double Cx = CX_MIN + iX * pixel_width;
      double Cy = CY_MIN + iY * pixel_height;

      double Zx = 0.0, Zy = 0.0, Zx2 = Zx * Zx, Zy2 = Zy * Zy;

      int i = 0;
      while (i < ITERATION_MAX && (Zx2 + Zy2) < ER2) {
        Zy = 2 * Zx * Zy + Cy;
        Zx = Zx2 - Zy2 + Cx;
        Zx2 = Zx * Zx;
        Zy2 = Zy * Zy;
        i++;
      }

      int common = iY * ix_max * 3 + iX * 3;
      if (i == ITERATION_MAX) {
        image_data[common] = 0;
        image_data[common + 1] = 0;
        image_data[common + 2] = 0;
      } else {
        // Prevent black color in the first thread
        int base = thread_num + 1;

        image_data[common + 0] = (base * COLOR_R) % 256;
        image_data[common + 1] = (base * COLOR_G) % 256;
        image_data[common + 2] = (base * COLOR_B) % 256;
      }
    }
  }
}

void generate_image() {
  auto start = std::chrono::high_resolution_clock::now();

  std::thread *threads = new std::thread[cpus];
  image_data = new unsigned char[ix_max * iy_max * 3];

  for (int i = 0; i < cpus; i++) {
    int start = i * chunk_size;
    int end = (i == cpus - 1) ? iy_max : (i + 1) * chunk_size;
    threads[i] = std::thread(computeMandelbrot, i, start, end);
  }

  for (int i = 0; i < cpus; i++) {
    threads[i].join();
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();

  std::cout << "Time: " << time << "ms\n";

  std::ofstream fp(FILENAME, std::ios::binary);
  fp << "P6\n # " << "\n " << ix_max << "\n " << iy_max << "\n " << 255 << '\n';
  fp.write((char *)image_data, ix_max * iy_max * 3);
  fp.close();

  delete[] image_data;
  delete[] threads;
}

int main(int argc, char **argv) {
  try {
    if (argc < 3) {
      std::cerr << "Usage: <cpus> <resolution>\n";
      return 1;
    }

    cpus = std::stoi(argv[1]);
    int res = std::stoi(argv[2]);

    if (cpus < 1 || res < 1) {
      std::cerr << "ERR: Number of CPUs/resolution cannot be less than 1.\n";
      return 1;
    }

    ix_max = res;
    iy_max = res;
    chunk_size = iy_max / cpus;

    pixel_width = (CX_MAX - CX_MIN) / ix_max;
    pixel_height = (CY_MAX - CY_MIN) / iy_max;

    std::cout << "Image " << FILENAME << " (" << ix_max << 'x' << iy_max
              << ")\n";

    generate_image();
  } catch (const std::exception &e) {
    std::cerr << "ERR: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
