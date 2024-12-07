#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>

class UlamSpiral {
public:
  void create(unsigned n, unsigned startWith = 1) {
    _lst.clear();
    if (!(n & 1))
      n++;
    _mx = n;
    unsigned v = n * n;
    _wd = static_cast<unsigned>(log10(static_cast<long double>(v))) + 1;
    for (unsigned u = 0; u < v; u++)
      _lst.push_back(-1);

    arrange(startWith);
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
    for (std::vector<unsigned>::iterator i = _lst.begin(); i != _lst.end();
         i++) {
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

  void arrange(unsigned s) {
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

// Równoległe przetwarzanie pozycji z blokowaniem 4x4
#pragma omp parallel for schedule(static, v / 16)
    for (unsigned idx = 0; idx < v; ++idx) {
      unsigned x = positions[idx].first;
      unsigned y = positions[idx].second;
      unsigned value = s + idx;
      _lst[x + y * _mx] = isPrime(value) ? value : 0;
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
};

int main(int argc, char *argv[]) {
  UlamSpiral ulam;
  // ulam.create(9);
  // ulam.display(0);
  // ulam.create(35);
  // ulam.display('#');

  ulam.create(512);
  ulam.generatePpm("ulam_spiral.ppm");

  return 0;
}
