#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

#include "common.hpp"

const int BACKLOG = 3;

void setSocketOptions(const int server_fd) {
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    handleError("setsockopt failed");
  }
}

void bindSocket(const int server_fd, const sockaddr_in &address) {
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    handleError("bind failed");
  }
}

void listenSocket(const int server_fd) {
  if (listen(server_fd, BACKLOG) < 0) {
    handleError("listen failed");
  }
}

int acceptConnection(int server_fd, sockaddr_in &address) {
  socklen_t addrlen = sizeof(address);
  int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
  if (new_socket < 0) {
    handleError("accept failed");
  }
  return new_socket;
}

int main(int argc, char const **argv) {
  int server_fd = createSocket();
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  setSocketOptions(server_fd);
  bindSocket(server_fd, address);
  listenSocket(server_fd);

  int new_socket = acceptConnection(server_fd, address);
  char buffer[BUFFER_SIZE];
  readData(new_socket, buffer);
  std::cout << buffer << '\n';

  std::string hello = "Hello from server";
  sendData(new_socket, hello);
  std::cout << "Hello message sent\n";

  close(new_socket);
  close(server_fd);
  return 0;
}
