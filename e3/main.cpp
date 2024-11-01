#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

const int WIDTH = 64;
const int HEIGHT = 64;

std::vector<std::vector<int>> maze(HEIGHT, std::vector<int>(WIDTH, 0));
std::mutex maze_mutex;
int thread_count = 1;

struct Position {
  int x;
  int y;
};

void generate_maze(const Position &pos, const int &thread_id) {
  std::vector<Position> directions = {
      {0, -1}, // left
      {0, 1},  // right
      {-1, 0}, // top
      {1, 0}   // bottom
  };

  std::random_shuffle(directions.begin(), directions.end());

  for (size_t i = 0; i < directions.size(); ++i) {
    Position new_pos = {pos.x + directions[i].x, pos.y + directions[i].y};

    // Boundary check
    if (new_pos.x >= 0 && new_pos.x < HEIGHT && new_pos.y >= 0 &&
        new_pos.y < WIDTH) {
      std::unique_lock<std::mutex> lock(maze_mutex);
      if (maze[new_pos.x][new_pos.y] == 0) {
        maze[new_pos.x][new_pos.y] = thread_id;
        lock.unlock();

        // If there are still routes available, start new threads for them
        if (i < directions.size() - 1) {
          for (size_t j = i + 1; j < directions.size(); ++j) {
            Position fork_pos = {pos.x + directions[j].x,
                                 pos.y + directions[j].y};
            if (fork_pos.x >= 0 && fork_pos.x < HEIGHT && fork_pos.y >= 0 &&
                fork_pos.y < WIDTH) {
              std::unique_lock<std::mutex> fork_lock(maze_mutex);
              if (maze[fork_pos.x][fork_pos.y] == 0) {
                thread_count++;
                int new_thread_id = thread_count;
                maze[fork_pos.x][fork_pos.y] = new_thread_id;
                fork_lock.unlock();
                std::thread(generate_maze, fork_pos, new_thread_id).detach();
              }
            }
          }
        }
        // Continue moving in the desired direction
        generate_maze(new_pos, thread_id);
        return;
      }
    }
  }
  // If the thread cannot move on, it terminates
}

void save_maze_to_ppm(const std::string &filename) {
  std::ofstream ofs(filename);
  ofs << "P3\n" << WIDTH << " " << HEIGHT << "\n255\n";
  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      int value = maze[i][j];
      int r = (value * 71) % 256;  // R
      int g = (value * 131) % 256; // G
      int b = (value * 197) % 256; // B
      ofs << r << " " << g << " " << b << " ";
    }
    ofs << "\n";
  }
  ofs.close();
}

int main() {
  std::srand(std::time(nullptr));
  Position start_pos = {HEIGHT / 2, WIDTH / 2};
  maze[start_pos.x][start_pos.y] = thread_count;
  std::thread main_thread(generate_maze, start_pos, thread_count);
  main_thread.join();

  save_maze_to_ppm("maze.ppm");
  std::cout << "Maze have been saved to maze.ppm\n";
  return 0;
}
