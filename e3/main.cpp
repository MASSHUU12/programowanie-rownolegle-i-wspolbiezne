#include <fstream>
#include <iostream>
#include <mutex>
#include <random>
#include <sys/types.h>
#include <thread>
#include <vector>

const short WALL = -1;
const short EMPTY = 0;
const u_int BOARD_LIMIT = 512;
const std::string FILENAME = "maze.ppm";

constexpr u_int COLOR_BASE = 50;
constexpr u_int COLOR_R = COLOR_BASE;
constexpr u_int COLOR_G = COLOR_BASE * 2;
constexpr u_int COLOR_B = COLOR_BASE * 3;

struct Cell {
  u_char value;
  u_char color[3];
  std::mutex mtx;

  Cell() : mtx(), value(EMPTY), color{0, 0, 0} {}
};

std::vector<Cell> board(BOARD_LIMIT * BOARD_LIMIT);
std::mutex children_mtx;

void generate_maze(const int x, const int y, const int tid,
                   std::vector<int> &children) {
  if (x < 0 || x >= BOARD_LIMIT || y < 0 || y >= BOARD_LIMIT) {
    return;
  }

  u_int cell_index = y * BOARD_LIMIT + x;
  std::lock_guard<std::mutex> lock(board[cell_index].mtx);
  if (board[cell_index].value == EMPTY) {
    board[cell_index].value = tid;

    u_short base = tid + 1;
    board[cell_index].color[0] = (base * COLOR_R) % 256;
    board[cell_index].color[1] = (base * COLOR_G) % 256;
    board[cell_index].color[2] = (base * COLOR_B) % 256;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 3);

  short directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  std::vector<short> availableDirections;

  for (u_short i = 0; i < 4; i++) {
    int nx = x + directions[i][0];
    int ny = y + directions[i][1];

    if (nx >= 0 && nx < BOARD_LIMIT && ny >= 0 && ny < BOARD_LIMIT) {
      if (board[ny * BOARD_LIMIT + nx].value == EMPTY) {
        availableDirections.push_back(i);
      }
    }
  }

  if (availableDirections.size() > 1) {
    for (int i = 1; i < availableDirections.size(); i++) {
      int nx = x + directions[availableDirections[i]][0];
      int ny = y + directions[availableDirections[i]][1];

      int new_tid = tid + children.size() + 1;

      std::lock_guard<std::mutex> lock(children_mtx);
      children.push_back(new_tid);
      std::thread t(generate_maze, nx, ny, new_tid, std::ref(children));
      t.detach();
    }
  }

  if (!availableDirections.empty()) {
    int nx = x + directions[availableDirections[0]][0];
    int ny = y + directions[availableDirections[0]][1];

    generate_maze(nx, ny, tid, std::ref(children));
  }
}

void generate_ppm() {
  std::ofstream fp(FILENAME, std::ios::binary);
  if (!fp) {
    std::cerr << "[ERR] Error opening file for writing.\n";
    return;
  }
  fp << "P6\n # \n " << BOARD_LIMIT << "\n " << BOARD_LIMIT << "\n 255\n";
  for (int y = 0; y < BOARD_LIMIT; y++) {
    for (int x = 0; x < BOARD_LIMIT; x++) {
      fp.write(reinterpret_cast<char *>(&board[y * BOARD_LIMIT + x].color), 3);
    }
  }
  fp.close();
}

int main(int argc, char **argv) {
  // tid liczyc od 1
  // liczyc potomkow, liczbe korytarzy, suma potomkow
  // jezeli sasiednie sa co najmniej 2 zera, tworzy nowy watek
  // mozna losowo

  // 0 0 0    0 2 0
  // 0 1 0 -> 3 1 1
  // 0 0 0    0 4 0

  std::vector<int> children;
  generate_maze(BOARD_LIMIT / 2, BOARD_LIMIT / 2, 1, std::ref(children));
  generate_ppm();

  return 0;
}
