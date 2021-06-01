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
    int client;
    sem_t *mutex;
};

//  Handle client
void * clientHandler(void *vargs) {
    //  Unpack arguments
    clientArgs *args = (clientArgs *)vargs;
    Log::Buffer &logBuffer = *(args->logBuffer);
    int client = args->client;
    sem_t *mutex = args->mutex;
    sem_post(mutex);
    std::cout << "Mutex released!" << std::endl;
    //  Read log messages and add them to buffer
    Log::Message msg;
    while(true) {
        Sock::readFrom(client, &msg, sizeof(msg));
        std::cout << "[" << msg.frameIndex << "] Source: " << (int)msg.sourceType << " Ticks: " << msg.ticks << std::endl;
        logBuffer.addMsg(msg);
    }
}

//  Arguments for hostSocket()
struct hostArgs {
    Config::Instance *config;
    Log::Buffer *logBuffer;
    sem_t *mutex;
};

//  Await for clients thread
void * hostSocket(void *vargs) {
    //  Unpack arguments
    hostArgs *args = (hostArgs *)vargs;
    Config::Instance &config = *(args->config);
    Log::Buffer &logBuffer = *(args->logBuffer);
    sem_t *mutex = args->mutex;
    //  Start socket
    int receiver = Sock::create();
    Sock::makeHost(receiver, config.serverIp, config.logPort);
    //  Client factory
    int newClient;
    while(true) {
        //  Await new client
        sem_wait(mutex);
        newClient = Sock::hostAwaitClient(receiver);
        //  Pack arguments and start a new thread
        clientArgs args = clientArgs{&logBuffer, newClient, mutex};
        pthread_t clientSock;
        pthread_create(&clientSock, NULL, clientHandler, (void *)&args);
    }
}

//  Entry point
int main(int argc, char* argv[]) {
    //  Initialize config, buffer and mutex
    Config::Instance config = Config::argsToConfig(argc, argv);
    Log::Buffer logBuffer;
    sem_t mutex;
    sem_init(&mutex, 0, 1);
    //  Pack arguments and start a thread
    hostArgs args = hostArgs{&config, &logBuffer, &mutex};
    pthread_t hostSock;
    pthread_create(&hostSock, NULL, hostSocket, (void *)&args);
    //  Terminate on ENTER
    std::cin.get();
    return 0;
}