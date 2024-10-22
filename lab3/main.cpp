#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <thread>

// Screen coordinate
int ix_max{};
int iy_max{};

// World coordinate = parameter plane
constexpr double CX_MIN = -2.5;
constexpr double CX_MAX = 1.5;
constexpr double CY_MIN = -2.0;
constexpr double CY_MAX = 2.0;

double pixel_width{};
double pixel_height{};

const std::string FILENAME = "new1.ppm";

// Iteration and bail-out value
const int ITERATION_MAX = 200;
constexpr double ESCAPE_RADIUS = 2;
constexpr double ER2 = ESCAPE_RADIUS * ESCAPE_RADIUS;

unsigned char *image_data;

int cpus{};
int chunk_size{};

void computeMandelbrot(int thread_num, int start, int end) {
  for (int iY = start; iY < end; iY++) {
    for (int iX = 0; iX < ix_max; iX++) {
      double Cx = CX_MIN + iX * pixel_width;
      double Cy = CY_MIN + iY * pixel_height;

      double Zx = 0.0;
      double Zy = 0.0;
      double Zx2 = Zx * Zx;
      double Zy2 = Zy * Zy;

      int Iteration = 0;
      while (Iteration < ITERATION_MAX && (Zx2 + Zy2) < ER2) {
        Zy = 2 * Zx * Zy + Cy;
        Zx = Zx2 - Zy2 + Cx;
        Zx2 = Zx * Zx;
        Zy2 = Zy * Zy;
        Iteration++;
      }

      int iX3 = iX * 3;
      int common = iY * ix_max * 3 + iX3;
      if (Iteration == ITERATION_MAX) {
        image_data[common] = 0;
        image_data[common + 1] = 0;
        image_data[common + 2] = 0;
      } else {
        // Prevent black color in the first thread
        int base = thread_num + 1;

        // Fancy colors
        int r = (base * 50) % 256;
        int g = (base * 100) % 256;
        int b = (base * 150) % 256;

        image_data[common] = r;
        image_data[common + 1] = g;
        image_data[common + 2] = b;
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
  } catch (const std::invalid_argument &e) {
    std::cerr << "ERR: Invalid argument.\n";
    return 1;
  } catch (const std::out_of_range &e) {
    std::cerr << "ERR: Out of range.\n";
    return 1;
  }

  return 0;
}
