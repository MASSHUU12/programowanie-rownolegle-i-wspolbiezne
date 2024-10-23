#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;
const int BACKLOG = 3;

void handleError(const std::string &message) {
  std::cerr << "[ERR]" << message << ": " << strerror(errno) << '\n';
  exit(1);
}

int createSocket() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    handleError("socket failed");
  }
  return server_fd;
}

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

void readData(int new_socket, char *buffer) {
  ssize_t valread = read(new_socket, buffer, BUFFER_SIZE - 1);
  if (valread < 0) {
    handleError("read failed");
  }
  buffer[valread] = '\0';
}

void sendData(int new_socket, const std::string &message) {
  if (send(new_socket, message.c_str(), message.length(), 0) < 0) {
    handleError("send failed");
  }
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
