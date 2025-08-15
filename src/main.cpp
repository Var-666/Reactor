
#include <iostream>
#include "mylib/InetAddress.h"
#include "mylib/EventLoop.h"
#include "mylib/Server.h"
#include "logger/Logger.h"
#include "logger/AsyncLogging.h"

int main() {
    InetAddress listenAddr(8080);
    EventLoop loop;

    //初始化异步日志系统
    auto asyncLogger = std::make_shared<AsyncLogging>("../Log/app",256*1024*1024, 3);
    Logger::instance().setAsyncLogger(asyncLogger);
    asyncLogger->start();
    Logger::instance().setLogLevel(LogLevel::DEBUG);

    LOG_INFO("Server is starting...");

    auto server = std::make_shared<Server>(&loop, listenAddr);
    server->setMessageCallback(
        [](const Connection::Ptr& conn, const std::string& msg) {
            LOG_INFO("New connection fd=%d", conn->fd());
            conn->send("Echo: " + msg);
        }
    );

    server->setConnectionCallback(
        [](const Connection::Ptr& conn) {
            LOG_INFO("New connection fd=%d", conn->fd());
        }
    );

    server->start();
    LOG_INFO("Server started on port 8080");
    loop.loop();

    asyncLogger->stop();
    LOG_INFO("Server stopped");
}