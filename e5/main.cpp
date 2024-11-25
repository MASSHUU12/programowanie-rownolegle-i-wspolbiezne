#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <vector>

const int WIDTH = 128, HEIGHT = 128;

std::vector<int> maze(HEIGHT *WIDTH, 0);
int descendant_count = 0, corridor_count = 1, max_path_length = 1;

struct Position {
  int x;
  int y;
};

bool is_valid_position(int x, int y) {
  return x >= 0 && x < HEIGHT && y >= 0 && y < WIDTH;
}

inline int to_maze_index(int x, int y) { return x * WIDTH + y; }

void generate_maze(const Position &pos, const int &thread_id, int path_length,
                   std::mt19937 &g) {
  std::vector<Position> directions = {
      {0, -1}, // left
      {0, 1},  // right
      {-1, 0}, // top
      {1, 0}   // bottom
  };

  std::shuffle(directions.begin(), directions.end(), g);

  for (size_t i = 0; i < directions.size(); ++i) {
    Position new_pos = {pos.x + directions[i].x, pos.y + directions[i].y};

    if (!is_valid_position(new_pos.x, new_pos.y)) {
      continue;
    }

    bool can_move = false;
#pragma omp critical
    {
      if (maze[to_maze_index(new_pos.x, new_pos.y)] == 0) {
        maze[to_maze_index(new_pos.x, new_pos.y)] = thread_id;
        can_move = true;
        corridor_count++;
      }
    }

    if (!can_move || i >= directions.size() - 1) {
      continue;
    }

    // Start new tasks for remaining directions
    for (size_t j = i + 1; j < directions.size(); ++j) {
      Position fork_pos = {pos.x + directions[j].x, pos.y + directions[j].y};
      if (!is_valid_position(fork_pos.x, fork_pos.y)) {
        continue;
      }

      bool can_fork = false;
      int new_thread_id = omp_get_thread_num() + 1;
#pragma omp critical
      {
        if (maze[to_maze_index(fork_pos.x, fork_pos.y)] == 0) {
          corridor_count++;
          // Disabling this line gives interesting results
          new_thread_id = corridor_count;
          maze[to_maze_index(fork_pos.x, fork_pos.y)] = new_thread_id;
          descendant_count++;
          can_fork = true;
        }
      }
      if (can_fork) {
#pragma omp task
        generate_maze(fork_pos, new_thread_id, path_length + 1, g);
      }
    }

    // Continue moving in the desired direction
    generate_maze(new_pos, thread_id, path_length + 1, g);
    return;
  }

#pragma omp critical
  max_path_length = std::max(path_length, max_path_length);
}

void save_maze_to_ppm(const std::string &filename) {
  std::ofstream ofs(filename);

  if (!ofs) {
    std::cerr << "Failed to open file " << filename << '\n';
    return;
  }

  ofs << "P3\n" << WIDTH << " " << HEIGHT << "\n255\n";
  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      int value = maze[to_maze_index(i, j)];
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
  double start_time = omp_get_wtime();
  Position start_pos = {HEIGHT / 2, WIDTH / 2};
  std::random_device rd;
  std::mt19937 g(rd());

  maze[to_maze_index(start_pos.x, start_pos.y)] = 1;

  omp_set_num_threads(omp_get_max_threads());

#pragma omp parallel
#pragma omp single
  generate_maze(start_pos, 1, 1, g);

  save_maze_to_ppm("maze.ppm");

  std::cout << "Maze have been saved to maze.ppm\n"
            << "\n-== Stats ==-\n"
            << "Number of descendants (threads created): " << descendant_count
            << "\nNumber of corridors (cells visited): " << corridor_count
            << "\nLongest corridor length: " << max_path_length
            << "\nGeneration time: " << (omp_get_wtime() - start_time) * 1000
            << "ms\n";

  return 0;
}
