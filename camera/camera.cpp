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

//  
void * cameraThread(void *vargs) {
    Config::Instance &config = *(Config::Instance *)vargs;

    int cameraSock = Sock::create();
    Sock::makeHost(cameraSock, config.serverIp, config.asciiPort);
    cameraSock = Sock::hostAwaitClient(cameraSock);

    int logSock = Sock::create();
    Sock::clientConnect(logSock, config.serverIp, config.logPort);

    int consoleSock = Sock::create();
    Sock::clientConnect(consoleSock, config.clientIp, config.consolePort);

    uint32_t frameIndex;
    uint8_t *picture = new uint8_t[config.pictureH * config.pictureW * 4];
    Log::Message msg;
    char *ascii = new char[config.consoleH * config.consoleW];

    while(true) {
        Sock::readFrom(cameraSock, &frameIndex, sizeof(frameIndex));
        Sock::readFrom(cameraSock, &picture, sizeof(uint8_t) * config.pictureH * config.pictureW * 4);

        msg = {Time::get(), frameIndex, Log::SrcPreASCII};
        Sock::writeTo(logSock, &msg, sizeof(msg));

        //  Here be dragons (store raw image with OpenCV, convert to text with img2txt)

        Sock::writeTo(consoleSock, ascii, sizeof(ascii));

        msg = {Time::get(), frameIndex, Log::SrcPostASCII};
        Sock::writeTo(logSock, &msg, sizeof(msg));
    }
}

//  Entry point
int main(int argc, char* argv[]) {
    Config::Instance config = Config::argsToConfig(argc, argv);

    pthread_t t;
    pthread_create(&t, NULL, cameraThread, (void *)&config);

    //  Terminate on ENTER
    std::cin.get();

    return 0;
}