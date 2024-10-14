#include <cmath>
#include <fstream>
#include <iostream>
#include <thread>

// Screen coordinate
const int IX_MAX = 800;
const int IY_MAX = 800;

// World coordinate = parameter plane
constexpr double CX_MIN = -2.5;
constexpr double CX_MAX = 1.5;
constexpr double CY_MIN = -2.0;
constexpr double CY_MAX = 2.0;

constexpr double PIXEL_WIDTH = (CX_MAX - CX_MIN) / IX_MAX;
constexpr double PIXEL_HEIGHT = (CY_MAX - CY_MIN) / IY_MAX;

// Color component (R or G or B) is coded from 0 to 255
// It is 24 bit color RGB file
const int MAX_COLOR_COMPONENT_VALUE = 255;

const std::string FILENAME = "new1.ppm";
const std::string COMMENT = "# ";

// Iteration and bail-out value
const int ITERATION_MAX = 200;
constexpr double ESCAPE_RADIUS = 2;
constexpr double ER2 = ESCAPE_RADIUS * ESCAPE_RADIUS;

unsigned char *IMAGE_DATA = new unsigned char[IX_MAX * IY_MAX * 3];

constexpr int NUM_THREADS = 4;
constexpr int CHUNK_SIZE = IY_MAX / NUM_THREADS;
std::thread threads[NUM_THREADS];

void computeMandelbrot(int start, int end) {
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
        IMAGE_DATA[iY * IX_MAX * 3 + iX * 3] = 255;
        IMAGE_DATA[iY * IX_MAX * 3 + iX * 3 + 1] = 255;
        IMAGE_DATA[iY * IX_MAX * 3 + iX * 3 + 2] = 255;
      }
    }
  }
}

int main() {
  for (int i = 0; i < NUM_THREADS; i++) {
    int start = i * CHUNK_SIZE;
    int end = (i == NUM_THREADS - 1) ? IY_MAX : (i + 1) * CHUNK_SIZE;
    threads[i] = std::thread(computeMandelbrot, start, end);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    threads[i].join();
  }

  std::ofstream fp("mandelbrot.ppm", std::ios::binary);
  fp << "P6\n " << COMMENT << "\n " << IX_MAX << "\n " << IY_MAX << "\n "
     << MAX_COLOR_COMPONENT_VALUE << '\n';
  fp.write((char *)IMAGE_DATA, IX_MAX * IY_MAX * 3);
  fp.close();

  delete[] IMAGE_DATA;

  return 0;
}
