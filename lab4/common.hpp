// common.hpp

#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <atomic>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> dataAvailable{false};

inline void handleError(const std::string &message)
{
    std::cerr << "[ERR] " << message << ": " << strerror(errno) << '\n';
    exit(1);
}

inline int createSocket()
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        handleError("Socket creation error");
    }
    return client_fd;
}

inline void sendData(const int socket, const std::string &message)
{
    if (send(socket, message.c_str(), message.length(), 0) < 0)
    {
        handleError("send failed");
    }
}

inline void readData(int socket, char *buffer)
{
    ssize_t valread = read(socket, buffer, BUFFER_SIZE - 1);
    if (valread < 0)
    {
        handleError("read failed");
    }
    buffer[valread] = '\0'; // Add null terminator
}

inline void listenForMessages(const int &socket)
{
    char buffer[BUFFER_SIZE] = {0};
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []
                    { return dataAvailable.load(); });
            dataAvailable = false;
            readData(socket, buffer);
        }
        if (std::string(buffer).find_first_not_of('\0') != std::string::npos)
        {
            std::cout << "Received message: " << buffer << '\n';
        }
        memset(buffer, 0, BUFFER_SIZE);
    }
}

inline void sendMessages(const int &socket)
{
    std::string message;
    while (true)
    {
        std::cout << "Enter message to send: ";
        std::getline(std::cin, message);
        {
            std::lock_guard<std::mutex> lock(mtx);
            sendData(socket, message);
            dataAvailable = true;
            cv.notify_one();
        }
        std::cout << "Message sent\n";
    }
}

#endif // COMMON_HPP
