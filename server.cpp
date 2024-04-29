#include <iostream>
#include <optional>
#include <limits>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

#include "io.h"
#include "cli.h"

class Server {
private:
    std::optional<int> sender_socket{};
    std::optional<int> receiver_socket{};

    int server_socket;
    bool is_closed = false;
public:
    explicit Server(unsigned short port) {
        server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket < 0) {
            throw std::runtime_error("Unable to create server socket");
        }

        sockaddr_in info{};
        memset(&info, 0, sizeof(info));
        info.sin_family = AF_INET;
        info.sin_addr.s_addr = htonl(INADDR_ANY);
        info.sin_port = htons(port);
        int res = bind(
                server_socket,
                reinterpret_cast<sockaddr *>(&info),
                sizeof(info)
        );
        if (res < 0) {
            throw std::runtime_error("Unable to bind server socket");
        }
        if (listen(server_socket, 5) < 0) {
            throw std::runtime_error("Unable to listen");
        }
        std::cout << "Init completed\n";
    }

private:
    void handshake() {
        std::cout << "Awaiting for clients\n";
        while (!sender_socket.has_value() || !receiver_socket.has_value()) {
            sockaddr_in c_info{};
            socklen_t c_info_len = sizeof(c_info);
            int c_socket = accept(
                    server_socket,
                    reinterpret_cast<sockaddr *>(&c_info),
                    &c_info_len
            );

            if (c_socket < 0) {
                throw std::runtime_error("Unable to accept connection");
            }

            std::string type = io::read(c_socket);

            if (type == "sender" && !sender_socket.has_value()) {
                sender_socket = c_socket;
                std::cout << "Sender connected. Address: " << inet_ntoa(c_info.sin_addr) << "\n";
            } else if (type == "receiver" && !receiver_socket.has_value()) {
                receiver_socket = c_socket;
                std::cout << "Receiver connected. Address: " << inet_ntoa(c_info.sin_addr) << "\n";
            } else {
                ::close(c_socket);
                std::cout << "Invalid client type\n";
            }
        }
        io::write(sender_socket.value(), "start");
        io::write(receiver_socket.value(), "start");
        std::cout << "Handshake finished\n";
    }

public:
    void handle() {
        handshake();

        std::cout << "Starting transition\n";
        std::string mes = io::read(sender_socket.value());
        std::cout << "Message: " << mes << "\n";
        io::write(receiver_socket.value(), mes);
        while (mes != "The End") {
            mes = io::read(sender_socket.value());
            io::write(receiver_socket.value(), mes);
        }
        std::cout << "Finishing...\n";
        close();
    }

    void close() {
        if (!is_closed) {
            is_closed = true;
            if (sender_socket.has_value()) {
                ::close(sender_socket.value());
            }
            if (receiver_socket.has_value()) {
                ::close(receiver_socket.value());
            }
        }
    }

    ~Server() {
        close();
    }
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw std::runtime_error("Wrong arguments count");
    }
    auto port = cli::parse_short(argv[1]);
    Server(port).handle();
}
