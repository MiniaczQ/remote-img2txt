#include <iostream>
#include <string>
#include <cstdint>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "../libs/config.cpp"
#include "../libs/log.cpp"
#include "../libs/socket.cpp"
#include "../libs/time.cpp"

//  Display ASCII art to console
void * displayInConsole(void *vargs) {
    Config::Instance &config = *(Config::Instance *)vargs;

    int asciiSock = Sock::create();
    Sock::makeHost(asciiSock, config.clientIp, config.consolePort);
    asciiSock = Sock::hostAwaitClient(asciiSock);

    int logSock = Sock::create();
    //Sock::clientConnect(logSock, config.serverIp, config.logPort);

    size_t dataInSize = sizeof(uint32_t) + config.consoleH * (config.consoleW + 1) * sizeof(char);
    uint8_t *dataIn = new uint8_t[dataInSize];
    Log::Message msg;
    while(true) {
        Sock::readFrom(asciiSock, dataIn, dataInSize);
        uint32_t frameIndex = ((uint32_t *)dataIn)[0];
        char *ascii = (char *)&dataIn[sizeof(uint32_t)];

        //  \x1B[2J
        std::cout << "\x1B[H" << ascii;
        std::cout.flush();

        msg = {Time::get(), frameIndex, Log::SrcConsole};
        //Sock::writeTo(logSock, &msg, sizeof(msg));
    }
    delete[] dataIn;
}

//  Entry point
int main(int argc, char* argv[]) {
    Config::Instance config = Config::argsToConfig(argc, argv);

    pthread_t t;
    pthread_create(&t, NULL, displayInConsole, (void *)&config);

    //  Terminate on ENTER
    std::cin.get();

    

    return 0;
}