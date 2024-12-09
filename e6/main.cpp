#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>

class UlamSpiral {
public:
  void create(unsigned n, unsigned startWith = 1,
              const std::string &method = "horizontal", int division = 2) {
    _lst.clear();
    if (!(n & 1))
      n++;
    _mx = n;
    _division = division;
    unsigned v = n * n;
    _wd = static_cast<unsigned>(log10(static_cast<long double>(v))) + 1;
    _lst.resize(v, -1);

    if (method == "quadrants") {
      arrangeQuadrants(startWith);
    } else {
      arrangeHorizontal(startWith);
    }
  }

  void display(char c) {
    if (!c)
      displayNumbers();
    else
      displaySymbol(c);
  }

  void generatePpm(const std::string &filename) {
    std::ofstream ofs(filename);
    unsigned ct = 0;

    if (!ofs) {
      std::cerr << "Failed to open file " << filename << '\n';
      return;
    }

    ofs << "P3\n" << _mx << " " << _mx << "\n255\n";
    for (auto i = _lst.begin(); i != _lst.end(); i++) {
      if (*i)
        ofs << "0 0 0 "; // Black for primes
      else
        ofs << "255 255 255 "; // White for non-primes
      if (++ct >= _mx) {
        ofs << '\n';
        ct = 0;
      }
    }
    ofs.close();
  }

private:
  bool isPrime(unsigned u) {
    if (u < 4)
      return u > 1;
    if (!(u % 2) || !(u % 3))
      return false;

    unsigned q = static_cast<unsigned>(sqrt(static_cast<long double>(u))),
             c = 5;
    while (c <= q) {
      if (!(u % c) || !(u % (c + 2)))
        return false;
      c += 6;
    }
    return true;
  }

  void arrangeHorizontal(unsigned s) {
    unsigned v = _mx * _mx;

    std::vector<std::pair<unsigned, unsigned>> positions(v);

    unsigned stp = 1, stC = 0;
    int dx = 1, dy = 0;
    unsigned posX = _mx >> 1, posY = posX;

    for (unsigned idx = 0; idx < v; ++idx) {
      positions[idx] = {posX, posY};

      if (dx) {
        posX += dx;
        if (++stC == stp) {
          dy = -dx;
          dx = 0;
          stC = 0;
        }
      } else {
        posY += dy;
        if (++stC == stp) {
          dx = dy;
          dy = 0;
          stC = 0;
          stp++;
        }
      }
    }

#pragma omp parallel for schedule(static)
    for (int y = 0; y < _mx; ++y) {
      for (unsigned x = 0; x < _mx; ++x) {
        unsigned idx = y * _mx + x;
        unsigned value = s + idx;
        _lst[x + y * _mx] = isPrime(value) ? value : 0;
      }
    }
  }

  void arrangeQuadrants(unsigned s) {
    unsigned v = _mx * _mx;

    std::vector<std::pair<unsigned, unsigned>> positions(v);

    unsigned stp = 1, stC = 0;
    int dx = 1, dy = 0;
    unsigned posX = _mx >> 1, posY = posX;

    for (unsigned idx = 0; idx < v; ++idx) {
      positions[idx] = {posX, posY};

      if (dx) {
        posX += dx;
        if (++stC == stp) {
          dy = -dx;
          dx = stC = 0;
        }
      } else {
        posY += dy;
        if (++stC == stp) {
          dx = dy;
          dy = stC = 0;
          stp++;
        }
      }
    }

    omp_set_nested(1);

#pragma omp parallel num_threads(_division)
    {
      int thread_num_y = omp_get_thread_num();
      int y_start = thread_num_y * (_mx / _division);
      int y_end = (thread_num_y + 1) * (_mx / _division);

#pragma omp parallel num_threads(_division)
      {
        int thread_num_x = omp_get_thread_num();
        int x_start = thread_num_x * (_mx / _division);
        int x_end = (thread_num_x + 1) * (_mx / _division);

        for (unsigned idx = 0; idx < v; ++idx) {
          unsigned x = positions[idx].first;
          unsigned y = positions[idx].second;

          if (x >= x_start && x < x_end && y >= y_start && y < y_end) {
            unsigned value = s + idx;
            _lst[x + y * _mx] = isPrime(value) ? value : 0;
          }
        }
      }
    }
  }

  void displayNumbers() {
    unsigned ct = 0;
    for (auto i = _lst.begin(); i != _lst.end(); i++) {
      if (*i)
        std::cout << std::setw(_wd) << *i << " ";
      else
        std::cout << std::string(_wd, '*') << " ";
      if (++ct >= _mx) {
        std::cout << "\n";
        ct = 0;
      }
    }
    std::cout << "\n\n";
  }

  void displaySymbol(char c) {
    unsigned ct = 0;
    for (auto i = _lst.begin(); i != _lst.end(); i++) {
      if (*i)
        std::cout << c;
      else
        std::cout << " ";
      if (++ct >= _mx) {
        std::cout << "\n";
        ct = 0;
      }
    }
    std::cout << "\n\n";
  }

  std::vector<unsigned> _lst;
  unsigned _mx, _wd;
  int _division;
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: <size> <horizontal|quadrants> [division]\n";
    return 1;
  }

  int size{}, division{2};
  std::string method{};

  try {
    size = std::stoi(argv[1]);
    method = argv[2];

    if (size <= 0) {
      std::cerr << "ERR: Invalid size. Size is less than 1.\n";
      return 1;
    }

    if (method != "horizontal" && method != "quadrants") {
      std::cerr << "ERR: Unknown method. Method is not 'horizontal' or "
                   "'quadrants'.\n";
      return 1;
    }

    if (method == "quadrants" && argc > 3) {
      division = std::stoi(argv[3]);

      if (division <= 0) {
        std::cerr << "ERR: Invalid division. Division is less than 1.\n";
        return 1;
      }
    }
  } catch (const std::invalid_argument &e) {
    std::cerr << "ERR: Invalid argument.\n";
    return 1;
  } catch (const std::out_of_range &e) {
    std::cerr << "ERR: Out of range.\n";
    return 1;
  }

  double start_time = omp_get_wtime();
  UlamSpiral ulam;

  ulam.create(size, 1, method, division);
  ulam.generatePpm("ulam_spiral.ppm");

  std::cout << "\nMethod: " << method << "\nSize: " << size
            << "\nTime: " << (omp_get_wtime() - start_time) * 1000 << "ms\n";

  return 0;
}
