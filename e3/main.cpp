#include <iostream>
#include <mutex>
#include <vector>
#include <fstream>
#include <thread>
#include <random>

const short WALL = -1;
const short EMPTY = 0;
const unsigned int BOARD_LIMIT = 256;
const std::string FILENAME = "maze.ppm";

struct Cell {
  unsigned short value;
  std::mutex mtx;
  unsigned char color[3];

  Cell() : mtx(), value(EMPTY), color{255, 0, 0} {}
};

Cell *board;

void generate_maze(int x, int y, int tid, std::vector<int> &children) {
  if (x < 0 || x >= BOARD_LIMIT || y < 0 || y >= BOARD_LIMIT) {
    return;
  }

  board[y * BOARD_LIMIT + x].value = tid;
  board[y * BOARD_LIMIT + x].color[0] = 255;
  board[y * BOARD_LIMIT + x].color[1] = 0;
  board[y * BOARD_LIMIT + x].color[2] = 0;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 3);

  int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  std::vector<int> availableDirections;

  for (int i = 0; i < 4; i++) {
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

      children.push_back(tid + children.size() + 1);
      std::thread t(generate_maze, nx, ny, tid + children.size() + 1, std::ref(children));
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
  fp << "P6\n # \n " << BOARD_LIMIT << "\n " << BOARD_LIMIT << "\n 255\n";
  for (int y = 0; y < BOARD_LIMIT; y++) {
    for (int x = 0; x < BOARD_LIMIT; x++) {
      fp.write((char *)&board[y * BOARD_LIMIT + x].color, 3);
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

  board = new Cell[BOARD_LIMIT * BOARD_LIMIT];

  std::vector<int> children;
  generate_maze(BOARD_LIMIT / 2, BOARD_LIMIT / 2, 1, std::ref(children));

  generate_ppm();

  delete[] board;

  return 0;
}
