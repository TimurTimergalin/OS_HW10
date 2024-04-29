#include <stdexcept>
#include <string>
#include <limits>
#include <iostream>

#include <sys/socket.h>
#include <cstring>

#include "Client.h"
#include "io.h"
#include "cli.h"


class Receiver : public Client {
public:
    Receiver(std::string ip, unsigned short port) : Client(std::move(ip), port) {}
protected:
    void handshake() override {
        std::cout << "Starting handshake\n";
        io::write(sock, "receiver");
        std::cout << "Awaiting server answer\n";
        if (io::read(sock) != "start") {
            throw std::runtime_error("Invalid handshake message from server");
        }
        std::cout << "Handshake finished\n";
    }

    void work() override {
        std::cout << "Starting to accept messages\n";

        while (true) {
            errno = 0;
            char buf[io::buf_size];
            ssize_t msg_size = recv(sock, buf, io::buf_size, 0);

            if (errno == 0) {
                if (strncmp(buf, "The End", io::buf_size) == 0) {
                    std::cout << "Finishing...\n";
                    break;
                }
                std::cout << "Message received: " << std::string(buf, msg_size) << "\n";
            } else if (errno == ENOTCONN){
                std::cout << "Finishing...\n";
                break;
            } else {
                throw std::runtime_error("Error while receiving");
            }
        }
    }
};


int main(int argc, char *argv[]) {
    auto [ip, port] = cli::parse_client_argv(argc, argv);
    Receiver(ip, port).handle();
}