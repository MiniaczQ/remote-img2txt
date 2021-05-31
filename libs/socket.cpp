#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdint>

//  Socket wraps
namespace sock {
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
        if (connect(c, (sockaddr *)&sAddr, sizeof(sAddr)) < 0) {
            std::cout << "Connection to server failed." << std::endl;
            throw -1;
        }
    }
}