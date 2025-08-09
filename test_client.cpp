#include <string>

const std::string server_ip = "172.16.97.131";
const int server_port = 8080;

#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include "mylib/Buffer.h"
#include "mylib/Codec.h"
#include "protocol/LengthHeaderProtocol.h"

void clientTask(const std::string& ip, uint16_t port, int id) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return;
    }

    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr);

    if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        close(sockfd);
        return;
    }

    Buffer inputBuffer;
    auto protocol = std::make_shared<LengthHeaderProtocol>();
    Codec codec(protocol);

    // 连续发送多条消息（制造粘包）
    std::vector<std::string> messages = {
        "Hello from client " + std::to_string(id) + " msg1",
        "Hello from client " + std::to_string(id) + " msg2",
        "Hello from client " + std::to_string(id) + " msg3"
    };

    // 拼接编码后的数据，一次性发出去，模拟粘包
    std::string sendData;
    for (auto& msg : messages) {
        sendData += codec.encode(msg);
    }

    ssize_t n = write(sockfd, sendData.data(), sendData.size());
    if (n < 0) {
        perror("write");
        close(sockfd);
        return;
    }
    std::cout << "Client " << id << " sent " << n << " bytes\n";

    // 接收服务器响应，拆包打印
    char buf[4096];
    while (true) {
        ssize_t nr = read(sockfd, buf, sizeof(buf));
        if (nr > 0) {
            inputBuffer.append(buf, nr);
            std::vector<std::string> recvMsgs;
            while (codec.decode(inputBuffer, recvMsgs)) {
                for (auto& m : recvMsgs) {
                    std::cout << "Client " << id << " recv: " << m << std::endl;
                }
                recvMsgs.clear();
            }
        } else if (nr == 0) {
            std::cout << "Client " << id << " server closed connection\n";
            break;
        } else {
            perror("read");
            break;
        }
    }

    close(sockfd);
}

int main() {
    // std::string serverIp = "127.0.0.1";
    // uint16_t serverPort = 8888;

    // 启动多个线程模拟多客户端连接
    const int clientCount = 5;
    std::vector<std::thread> clients;

    for (int i = 0; i < clientCount; ++i) {
        clients.emplace_back(clientTask, server_ip, server_port, i + 1);
    }

    for (auto& t : clients) {
        t.join();
    }

    return 0;
}