#ifndef OS_HW10_CLI_H
#define OS_HW10_CLI_H

namespace cli {
    struct ClientCommandLineArguments {
        std::string ip;
        unsigned short port;

        ClientCommandLineArguments &operator=(ClientCommandLineArguments &&other) noexcept {
            ip = std::move(other.ip);
            port = other.port;
            return *this;
        }
    };

    unsigned short parse_short(char *port_s) {
        int iport = std::stoi(port_s);
        if (iport > std::numeric_limits<unsigned short>::max() ||
            iport < std::numeric_limits<unsigned short>::min()) {
            throw std::runtime_error("Invalid port value");
        }
        return static_cast<unsigned short>(iport);
    }

    ClientCommandLineArguments parse_client_argv(int argc, char *argv[]) {
        if (argc != 3) {
            throw std::runtime_error("Wrong arguments count");
        }

        std::string ip = argv[1];
        auto port = parse_short(argv[2]);

        return {ip, port};
    }
}

#endif //OS_HW10_CLI_H
