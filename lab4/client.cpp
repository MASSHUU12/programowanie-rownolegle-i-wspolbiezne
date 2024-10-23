#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

void handleError(const std::string &message) {
  std::cerr << "[ERR]" << message << ": " << strerror(errno) << '\n';
  exit(1);
}

int createSocket() {
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd < 0) {
    handleError("Socket creation error");
  }
  return client_fd;
}

void setupServerAddress(sockaddr_in &serv_addr) {
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    handleError("Invalid address/ Address not supported");
  }
}

void connectToServer(const int client_fd, const sockaddr_in &serv_addr) {
  int status =
      connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (status < 0) {
    handleError("Connection Failed");
  }
}

void sendMessage(const int client_fd, const std::string message) {
  int bytesSent = send(client_fd, message.c_str(), message.length(), 0);
  if (bytesSent < 0) {
    handleError("Error sending message");
  }
  std::cout << "Message sent\n";
}

void receiveMessage(const int client_fd, char *buffer) {
  int bytesReceived = read(client_fd, buffer, BUFFER_SIZE - 1);
  if (bytesReceived < 0) {
    handleError("Error receiving message");
  }
  buffer[bytesReceived] = '\0'; // Add null terminator
  std::cout << buffer << std::endl;
}

int main(const int argc, char const **argv) {
  int client_fd = createSocket();
  struct sockaddr_in serv_addr;
  setupServerAddress(serv_addr);
  connectToServer(client_fd, serv_addr);

  const std::string hello = "Hello from client";
  sendMessage(client_fd, hello);

  char buffer[BUFFER_SIZE] = {0};
  receiveMessage(client_fd, buffer);

  close(client_fd);
  return 0;
}
