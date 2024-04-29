#include "Client.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <utility>
#include <stdexcept>
#include <iostream>
#include <cstring>

Client::Client(std::string ip, unsigned short port) : ip(std::move(ip)), port(port), sock(-1) {}

void Client::connect() {
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        throw std::runtime_error("Unable to create socket");
    }

    std::cout << "Connecting to the server\n";
    sockaddr_in info{};
    memset(&info, 0, sizeof(info));
    info.sin_family = AF_INET;
    info.sin_port = htons(port);

    if (inet_aton(ip.c_str(), &info.sin_addr) < 0) {
        throw std::runtime_error("Invalid ip address");
    }

    int conn = ::connect(
            sock,
            reinterpret_cast<sockaddr *>(&info),
            sizeof(info)
    );
    if (conn < 0) {
        throw std::runtime_error("Unable to connect to the server");
    }
}

Client::~Client() {
    close();
}

void Client::close() {
    if (!is_closed) {
        is_closed = true;
        ::close(sock);
    }
}

void Client::handle() {
    connect();
    handshake();
    work();
    close();
}



