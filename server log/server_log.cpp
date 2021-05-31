#include <iostream>
#include <string>
#include <cstdint>
#include <string.h>
#include <fstream>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#include "../libs/config.cpp"
#include "../libs/log.cpp"
#include "../libs/socket.cpp"

//  Arguments for clientHandler()
struct clientArgs {
    LogBuffer *logB;
    int *client;
};

//  Handle client
void *clientHandler(void* vargs) {
    clientArgs *args = (clientArgs *)vargs;
    LogBuffer &logB = *(args->logB);
    int &client = *(args->client);

    LogMsg msg;
    while(true) {
        int res = read(client, &msg, sizeof(msg));
        if (res <= 0) {
            std::cout << "Conneciton lost." << std::endl;
            throw -1;
        }
        if (res > 0)
            logB.addMsg(msg);
    }
}

//  Arguments for hostSocket()
struct hostArgs {
    Config *cfg;
    LogBuffer *logB;
};

//  Await for clients thread
void * hostSocket(void *vargs) {
    hostArgs *args = (hostArgs *)vargs;
    Config &cfg = *(args->cfg);
    LogBuffer &logB = *(args->logB);

    int receiver = sock::create();
    sock::makeHost(receiver, cfg.serverIp, cfg.logPort);

    while(true) {
        //  Client factory
        int newClient;
        newClient = sock::hostAwaitClient(receiver);

        clientArgs args = clientArgs{&logB, &newClient};
        pthread_t clientSock;
        pthread_create(&clientSock, NULL, clientHandler, (void *)&args);
    }
}

//  Entry point
int main(int argc, char* argv[]) {
    Config cfg = getConfig(argc, argv);
    LogBuffer logB;
    
    hostArgs args = hostArgs{&cfg, &logB};
    pthread_t hostSock;
    pthread_create(&hostSock, NULL, hostSocket, (void *)&args);

    //  Terminate on ENTER
    std::cin.get();

    return 0;
}