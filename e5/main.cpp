#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <vector>

const int WIDTH = 128, HEIGHT = 128;

std::vector<std::vector<int>> maze(HEIGHT, std::vector<int>(WIDTH, 0));
int thread_count = 1, descendant_count = 0, corridor_count = 0,
    max_path_length = 0;

struct Position {
  int x;
  int y;
};

void generate_maze(const Position &pos, const int &thread_id, int path_length) {
  std::vector<Position> directions = {
      {0, -1}, // left
      {0, 1},  // right
      {-1, 0}, // top
      {1, 0}   // bottom
  };

  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(directions.begin(), directions.end(), g);

  for (size_t i = 0; i < directions.size(); ++i) {
    Position new_pos = {pos.x + directions[i].x, pos.y + directions[i].y};

    // Boundary check
    if (new_pos.x >= 0 && new_pos.x < HEIGHT && new_pos.y >= 0 &&
        new_pos.y < WIDTH) {
      bool can_move = false;
#pragma omp critical
      {
        if (maze[new_pos.x][new_pos.y] == 0) {
          maze[new_pos.x][new_pos.y] = thread_id;
          can_move = true;
          corridor_count++;
        }
      }

      if (can_move) {
        // Start new tasks for remaining directions
        if (i < directions.size() - 1) {
          for (size_t j = i + 1; j < directions.size(); ++j) {
            Position fork_pos = {pos.x + directions[j].x,
                                 pos.y + directions[j].y};
            if (fork_pos.x >= 0 && fork_pos.x < HEIGHT && fork_pos.y >= 0 &&
                fork_pos.y < WIDTH) {
              bool can_fork = false;
              int new_thread_id = 0;
#pragma omp critical
              {
                if (maze[fork_pos.x][fork_pos.y] == 0) {
                  new_thread_id = ++thread_count;
                  maze[fork_pos.x][fork_pos.y] = new_thread_id;
                  can_fork = true;
                  descendant_count++;
                  corridor_count++;
                }
              }
              if (can_fork) {
#pragma omp task
                generate_maze(fork_pos, new_thread_id, path_length + 1);
              }
            }
          }
        }
        // Continue moving in the desired direction
        generate_maze(new_pos, thread_id, path_length + 1);
        return;
      }
    }
  }
// Update max path length if necessary
#pragma omp critical
  {
    if (path_length > max_path_length) {
      max_path_length = path_length;
    }
  }
}

void save_maze_to_ppm(const std::string &filename) {
  std::ofstream ofs(filename);
  ofs << "P3\n" << WIDTH << " " << HEIGHT << "\n255\n";
  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      int value = maze[i][j];
      int r = (value * 71) % 256;
      int g = (value * 131) % 256;
      int b = (value * 197) % 256;
      ofs << r << " " << g << " " << b << " ";
    }
    ofs << "\n";
  }
  ofs.close();
}

int main() {
  Position start_pos = {HEIGHT / 2, WIDTH / 2};
  maze[start_pos.x][start_pos.y] = thread_count;
  corridor_count++;

#pragma omp parallel
#pragma omp single
  generate_maze(start_pos, thread_count, 1);

  save_maze_to_ppm("maze.ppm");

  std::cout << "Maze have been saved to maze.ppm\n"
            << "\n-== Stats ==-\n"
            << "Number of descendants (threads created): " << descendant_count
            << "\nNumber of corridors (cells visited): " << corridor_count
            << "\nLongest corridor length: " << max_path_length << '\n';

  return 0;
}
