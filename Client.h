#ifndef OS_HW10_CLIENT_H
#define OS_HW10_CLIENT_H


#include <string>

class Client {
private:
    bool is_closed;
protected:
    std::string ip;
    unsigned short port;
    int sock;
public:
    Client(std::string ip, unsigned short port);

    void handle();

    void close();

    virtual ~Client();

private:
    void connect();

protected:
    virtual void handshake() = 0;

    virtual void work() = 0;
};


#endif //OS_HW10_CLIENT_H
