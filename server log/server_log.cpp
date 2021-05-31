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
#include "../libs/socket.cpp"

//  Arguments for hostSocket()
struct hostArgs {
    Config *cfg;
    LogBuffer *logB;
};

//  Await for clients thread
void *hostSocket(void* vargs) {
    hostArgs *args = (hostArgs*)vargs;
    Config &cfg = *(args->cfg);
    LogBuffer &logB = *(args->logB);

    int receiver = sock::create();
    sock::makeHost(receiver, cfg.serverIp, cfg.logPort);

    while(true) {
        int newClient;
        newClient = sock::hostAwaitClient(receiver);
    }
}

//  Entry point
int main(int argc, char* argv[]) {
    Config cfg = getConfig(argc, argv);
    LogBuffer logB;
    
    hostArgs args = hostArgs{&cfg, &logB};
    pthread_t hostSock;
    pthread_create(&hostSock, NULL, hostSocket, (void *)&args);

    std::cin.get();

    return 0;
}