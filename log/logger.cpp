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
    Log::Buffer *logBuffer;
    int *client;
};

//  Handle client
void * clientHandler(void *vargs) {
    clientArgs *args = (clientArgs *)vargs;
    Log::Buffer &logBuffer = *(args->logBuffer);
    int &client = *(args->client);

    Log::Message msg;
    while(true) {
        Sock::readFrom(client, &msg, sizeof(msg));
        logBuffer.addMsg(msg);
    }
}

//  Arguments for hostSocket()
struct hostArgs {
    Config::Instance *config;
    Log::Buffer *logBuffer;
};

//  Await for clients thread
void * hostSocket(void *vargs) {
    hostArgs *args = (hostArgs *)vargs;
    Config::Instance &config = *(args->config);
    Log::Buffer &logBuffer = *(args->logBuffer);

    int receiver = Sock::create();
    Sock::makeHost(receiver, config.serverIp, config.logPort);

    while(true) {
        //  Client factory
        int newClient;
        newClient = Sock::hostAwaitClient(receiver);

        clientArgs args = clientArgs{&logBuffer, &newClient};
        pthread_t clientSock;
        pthread_create(&clientSock, NULL, clientHandler, (void *)&args);
    }
}

//  Entry point
int main(int argc, char* argv[]) {
    Config::Instance config = Config::argsToConfig(argc, argv);
    Log::Buffer logBuffer;
    
    hostArgs args = hostArgs{&config, &logBuffer};
    pthread_t hostSock;
    pthread_create(&hostSock, NULL, hostSocket, (void *)&args);

    //  Terminate on ENTER
    std::cin.get();

    return 0;
}