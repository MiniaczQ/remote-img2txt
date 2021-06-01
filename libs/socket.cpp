#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdint>
#include <unistd.h>

#define MAX_ATTEMPTS 30
#define ATTEMPTS_DELAY 1000000

//  Socket wraps
namespace Sock {
    //  Create socket address from family, ip and port
    sockaddr_in getAddr(sa_family_t family, std::string ip, uint16_t port) {
        sockaddr_in addr;
        addr.sin_family = family;
        addr.sin_port = htons(port);
        if (inet_pton(family, ip.c_str(), &addr.sin_addr) <= 0) {
            std::cout << "Invalid IP string." << std::endl;
            throw -1;
        }
        return addr;
    }

    //  Create socket
    int create() {
        int s;
        if ((s = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            std::cout << "Failed to create receiver socket." << std::endl;
            throw -1;
        }
        return s;
    }

    //  Configure host socket
    void makeHost(int s, std::string ip, uint16_t port) {
        int opt = 1;
        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            std::cout << "Failed to configure socket." << std::endl;
            throw -1;
        }

        sockaddr_in address = getAddr(AF_INET, ip, port);

        if (bind(s, (sockaddr *)&address, sizeof(address)) < 0) {
            std::cout << "Failed to bind socket." << std::endl;
            throw -1;
        }
    }

    //  Host socket listen and accept
    int hostAwaitClient(int s) {
        int c;
        if (listen(s, 3) < 0) {
            std::cout << "Failed to listen for incoming clients." << std::endl;
            throw -1;
        }
        if ((c = accept(s, NULL, NULL)) < 0) {
            std::cout << "Failed to accept incoming client." << std::endl;
            throw -1;
        }
        return c;
    }

    //  Client connect to host socket
    void clientConnect(int c, std::string ip, uint16_t port) {
        sockaddr_in sAddr = getAddr(AF_INET, ip, port);

        int attempt_counter = 0;

        while (connect(c, (sockaddr *)&sAddr, sizeof(sAddr)) < 0) {
            if (attempt_counter < MAX_ATTEMPTS)
                usleep(ATTEMPTS_DELAY);
            else {
                std::cout << "Connection to server failed after many attempts." << std::endl;
                throw -1;
            }
        }
    }

    //  Read wrap
    ssize_t readFrom(int __fd, void *__buf, size_t __nbytes) {
        ssize_t r = 0;
        uint8_t *p = (uint8_t *)__buf;
        while(r != __nbytes) {
            ssize_t res = read(__fd, p + r, __nbytes - r);
            if (res <= 0) {
                std::cout << "Conneciton lost." << std::endl;
                throw -1;
            }
            r += res;
        }
        return r;
    }

    //  Write wrap
    ssize_t writeTo(int __fd, void *__buf, size_t __nbytes) {
        ssize_t res = write(__fd, __buf, __nbytes);
        if (res <= 0) {
            std::cout << "Conneciton lost." << std::endl;
            throw -1;
        }
        return res;
    }

    //  Create server socket and return socket to the first client
    int fullHostOne(std::string ip, uint16_t port) {
        int s = Sock::create();
        Sock::makeHost(s, ip, port);
        return Sock::hostAwaitClient(s);
    }

    //  Create client socket and return it when connected to a server
    int fullConnectTo(std::string ip, uint16_t port) {
        int s = Sock::create();
        Sock::clientConnect(s, ip, port);
        return s;
    }
}