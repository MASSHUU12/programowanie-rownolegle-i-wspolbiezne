#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

#include "common.hpp"

void setupServerAddress(sockaddr_in &serv_addr)
{
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    handleError("Invalid address/ Address not supported");
  }
}

void connectToServer(const int &client_fd, const sockaddr_in &serv_addr)
{
  int status =
      connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (status < 0)
  {
    handleError("Connection Failed");
  }
}

int main(const int argc, char const **argv)
{
  int client_fd = createSocket();
  sockaddr_in serv_addr;
  setupServerAddress(serv_addr);
  connectToServer(client_fd, serv_addr);

  std::thread listener(listenForMessages, client_fd);
  std::thread sender(sendMessages, client_fd);

  listener.join();
  sender.join();

  close(client_fd);
  return 0;
}
