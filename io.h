#ifndef OS_HW10_IO_H
#define OS_HW10_IO_H

#include <string>
#include <sys/socket.h>
#include <stdexcept>

namespace io {
    const int buf_size = 32;

    std::string read(int socket) {
        char buf[buf_size];
        ssize_t msg_size = recv(socket, buf, buf_size, 0);
        if (msg_size < 0) {
            throw std::runtime_error("Unable to receive a message");
        }
        return {buf, static_cast<size_t>(msg_size)};
    }

    void write(int socket, const std::string &mes) {
        if (send(socket, mes.c_str(), mes.size(), 0) != mes.size()) {
            throw std::runtime_error("Unable to send whole message");
        }
    }
}
#endif //OS_HW10_IO_H
