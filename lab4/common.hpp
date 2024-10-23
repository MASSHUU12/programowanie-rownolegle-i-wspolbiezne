// common.hpp

#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

inline void handleError(const std::string &message) {
  std::cerr << "[ERR]" << message << ": " << strerror(errno) << '\n';
  exit(1);
}

inline int createSocket() {
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd < 0) {
    handleError("Socket creation error");
  }
  return client_fd;
}

inline void sendData(const int socket, const std::string &message) {
  if (send(socket, message.c_str(), message.length(), 0) < 0) {
    handleError("send failed");
  }
}

inline void readData(int socket, char *buffer) {
  ssize_t valread = read(socket, buffer, BUFFER_SIZE - 1);
  if (valread < 0) {
    handleError("read failed");
  }
  buffer[valread] = '\0'; // Add null terminator
}

#endif // COMMON_HPP
