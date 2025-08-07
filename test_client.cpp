//
// Created by Var on 25-8-2.
//
// test_client.cpp

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <chrono>

//客户端线程函数
void clientThread(int id, const std::string& server_ip, int server_port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Client " << id << " socket creation failed\n";
        return;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Client " << id << " invalid address\n";
        close(sockfd);
        return;
    }

    if (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Client " << id << " connect failed\n";
        close(sockfd);
        return;
    }

    std::cout << "Client " << id << " connected to server\n";

    for (int i = 0; i < 10; ++i) {
        std::string msg = "Client " + std::to_string(id) + ": hello " + std::to_string(i) + "\n";
        send(sockfd, msg.c_str(), msg.size(), 0);

        char buf[1024];
        ssize_t n = recv(sockfd, buf, sizeof(buf) - 1, 0);
        if (n > 0) {
            buf[n] = '\0';
            std::cout << "Client " << id << " received: " << buf;
        } else if (n == 0) {
            std::cerr << "Client " << id << " server closed connection\n";
            break;
        } else {
            std::cerr << "Client " << id << " recv failed\n";
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟延迟
    }

    close(sockfd);
    std::cout << "Client " << id << " disconnected\n";
}

int main() {
    const std::string server_ip = "172.16.97.131";
    const int server_port = 8080;
    const int client_count = 5;  // 并发连接数量

    std::vector<std::thread> threads;
    for (int i = 0; i < client_count; ++i) {
        threads.emplace_back(clientThread, i, server_ip, server_port);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "All clients finished.\n";
    return 0;
}
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#include "mylib/Timer.h"

// int main() {
//     int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     sockaddr_in servaddr{};
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_port = htons(8080);  // 服务器端口
//     inet_pton(AF_INET, "172.16.97.131", &servaddr.sin_addr);
//
//     if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
//         std::cerr << "Connect failed\n";
//         return 1;
//     }
//
//     std::cout << "Connected to server\n";
//
//     for (int i = 0; i < 5; ++i) {
//         const char* msg = "hello\n";
//         send(sockfd, msg, strlen(msg), 0);
//         std::cout << "Sent message: " << msg;
//         sleep(2);  // 每2秒发一次，保持活跃
//     }
//     // sleep(20);
//     std::cout << "Closing connection\n";
//     close(sockfd);
//     return 0;
// }

