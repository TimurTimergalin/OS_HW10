#include <stdexcept>
#include <string>
#include <limits>
#include <iostream>

#include <sys/socket.h>

#include "Client.h"
#include "io.h"
#include "cli.h"

class Sender : public Client {
public:
    Sender(std::string ip, unsigned short port): Client(std::move(ip), port) {}
protected:
    void handshake() override {
        std::cout << "Starting handshake\n";
        io::write(sock, "sender");
        std::cout << "Awaiting server answer\n";
        if (io::read(sock) != "start") {
            throw std::runtime_error("Invalid handshake message from server");
        }
        std::cout << "Handshake finished\n";
    }

    void work() override {
        std::cout << "Starting to accept messages\n";
        while (true) {
            std::string mes;
            std::getline(std::cin, mes);

            errno = 0;
            send(sock, mes.c_str(), mes.size(), 0);

            if (errno != 0) {
                if (errno == ENOTCONN) {
                    std::cout << "Finishing...\n";
                    break;
                } else {
                    throw std::runtime_error("Error while sending");
                }
            } else if (mes == "The End") {
                std::cout << "Finishing...\n";
                break;
            }
        }
    }
};

int main(int argc, char *argv[]) {
    auto [ip, port] = cli::parse_client_argv(argc, argv);
    Sender(ip, port).handle();
}