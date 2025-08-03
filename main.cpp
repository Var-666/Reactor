
#include <iostream>
#include "InetAddress.h"
#include "EventLoop.h"
#include "Server.h"

int main() {
    InetAddress listenAddr(8080);
    EventLoop loop;

    Server server(&loop, listenAddr);
    server.setNumThreads(4);
    server.setMessageCallback(
        [](const Connection::Ptr& conn, const std::string& msg) {
            std::cout << "Received: " << msg << std::endl;
            conn->send("Echo: " + msg);
        }
    );

    server.setConnectionCallback(
        [](const Connection::Ptr& conn) {
            std::cout << "New connection: fd = " << conn->fd() << std::endl;
        }
    );

    server.start();
    loop.loop();
}