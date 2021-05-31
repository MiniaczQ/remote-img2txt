#include <iostream>
#include <string>
#include <cstdint>
#include <string.h>
#include <fstream>
#include <vector>

#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#include "../libs/config.cpp"
#include "../libs/log.cpp"

struct hostArgs {
    Config *cfg;
    LogBuffer *logB;
};

void *hostSocket(void* vargs) {
    hostArgs *args = (hostArgs*)vargs;
    while(true) {
        std::cout << "test" << std::endl;
        usleep(1000000);
    }
}

//  Entry point
int main(int argc, char* argv[]) {
    Config cfg = getConfig(argc, argv);
    LogBuffer logB;
    hostArgs args = hostArgs{&cfg, &logB};
    pthread_t hostSock;

    pthread_create(&hostSock, NULL, hostSocket, (void *)&args);

    std::string a;
    std::cin >> a;

    return 0;
}