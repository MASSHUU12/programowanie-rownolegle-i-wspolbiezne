#include <cmath>
#include <fstream>
#include <iostream>

int main() {
  // Screen coordinate
  int iX, iY;
  const int iXmax = 800;
  const int iYmax = 800;

  // World coordinate = parameter plane
  double Cx, Cy;
  constexpr double CxMin = -2.5;
  constexpr double CxMax = 1.5;
  constexpr double CyMin = -2.0;
  constexpr double CyMax = 2.0;

  constexpr double PixelWidth = (CxMax - CxMin) / iXmax;
  constexpr double PixelHeight = (CyMax - CyMin) / iYmax;

  // Color component (R or G or B) is coded from 0 to 255
  // It is 24 bit color RGB file
  const int MaxColorComponentValue = 255;

  const std::string filename = "new1.ppm";
  const std::string comment = "# ";

  // Z = Zx + Zy * i ; Z0 = 0
  double Zx, Zy;
  double Zx2, Zy2;

  // Iteration and bail-out value
  int Iteration;
  const int IterationMax = 200;
  constexpr double EscapeRadius = 2;
  constexpr double ER2 = EscapeRadius * EscapeRadius;

  // Create new file and open it in binary mode
  std::ofstream fp(filename, std::ios::binary);

  // Write ASCII header to the file
  fp << "P6\n " << comment << "\n " << iXmax << "\n " << iYmax << "\n "
     << MaxColorComponentValue << '\n';

  unsigned char imageData[iXmax * iYmax * 3];

  // Compute and write image data bytes to the file
  int imageDataIndex = 0;
  for (iY = 0; iY < iYmax; iY++) {
    Cy = CyMin + iY * PixelHeight;
    if (std::fabs(Cy) < PixelHeight / 2)
      Cy = 0.0; // Main antenna

    for (iX = 0; iX < iXmax; iX++) {
      Cx = CxMin + iX * PixelWidth;

      // Initial value of orbit = critical point Z= 0
      Zx = 0.0;
      Zy = 0.0;
      Zx2 = Zx * Zx;
      Zy2 = Zy * Zy;

      // Iterate until bail-out value or max iterations
      for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2);
           Iteration++) {
        Zy = 2 * Zx * Zy + Cy;
        Zx = Zx2 - Zy2 + Cx;
        Zx2 = Zx * Zx;
        Zy2 = Zy * Zy;
      };

      // Compute pixel color (24 bit = 3 bytes)
      if (Iteration == IterationMax) {
        // Interior of Mandelbrot set = black
        imageData[imageDataIndex++] = 0;
        imageData[imageDataIndex++] = 0;
        imageData[imageDataIndex++] = 0;
      } else {
        // Exterior of Mandelbrot set = white
        imageData[imageDataIndex++] = 255; // Red
        imageData[imageDataIndex++] = 255; // Green
        imageData[imageDataIndex++] = 255; // Blue
      };
    }
  }

  fp.write((char *)imageData, iXmax * iYmax * 3);
  fp.close();
  return 0;
}
