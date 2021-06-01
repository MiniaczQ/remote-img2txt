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
    //  Unpack arguments
    Config::Instance &config = *(Config::Instance *)vargs;
    //  Connect sockets
    int asciiSock = Sock::fullHostOne(config.clientIp, config.consolePort);
    int logSock = Sock::fullConnectTo(config.serverIp, config.logPort);
    //  Allocate loop variables
    size_t asciiSize = config.consoleH * (config.consoleW + 1) * sizeof(char);
    uint8_t *ascii = new uint8_t[asciiSize];
    Log::Message msg;
    uint32_t frameIndex;
    //  Allocate input buffer
    size_t inDataSize = sizeof(frameIndex) + config.consoleH * (config.consoleW + 1);
    char *inData = new char[inDataSize];
    //  Repeat until terminated
    while (true) {
        //  Receive data
        Sock::readFrom(asciiSock, inData, inDataSize);
        frameIndex = ((uint32_t *)inData)[0];
        //  Display
        //inData[inDataSize] = 0;
        std::cout << "\x1B[H" << &inData[sizeof(frameIndex)];
        std::cout.flush();
        //  Log
        msg = {Time::get(), frameIndex, Log::SrcConsole};
        Sock::writeTo(logSock, &msg, sizeof(msg));
    }
    delete[] ascii;
}

//  Entry point
int main(int argc, char* argv[]) {
    //  Initialize config
    Config::Instance config = Config::argsToConfig(argc, argv);
    //  Pack arguments and start a thread
    pthread_t t;
    pthread_create(&t, NULL, displayInConsole, (void *)&config);
    //  Terminate on ENTER
    std::cin.get();
    return 0;
}