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
void * pictureToASCII(void *vargs) {
    Config::Instance &config = *(Config::Instance *)vargs;

    int cameraSock = Sock::create();
    //Sock::makeHost(cameraSock, config.serverIp, config.asciiPort);
    //cameraSock = Sock::hostAwaitClient(cameraSock);

    int logSock = Sock::create();
    //Sock::clientConnect(logSock, config.serverIp, config.logPort);

    int consoleSock = Sock::create();
    Sock::clientConnect(consoleSock, config.clientIp, config.consolePort);

    size_t dataInSize = sizeof(uint32_t) + config.pictureH * config.pictureW * 4;
    uint8_t *dataIn = new uint8_t[dataInSize]; 
    Log::Message msg;
    size_t dataOutSize = sizeof(uint32_t) + config.consoleH * (config.consoleW + 1);
    char *dataOut = new char[dataOutSize];
    while(true) {
        //Sock::readFrom(cameraSock, dataIn, dataInSize);
        uint32_t frameIndex = ((uint32_t *)dataIn)[0];
        uint8_t *picture = &dataIn[sizeof(uint32_t)];

        msg = {Time::get(), frameIndex, Log::SrcPreASCII};
        //Sock::writeTo(logSock, &msg, sizeof(msg));

        //  Here be dragons (store raw image with OpenCV, convert to text with img2txt)
        
        //  TEMPORARY
        char *ascii = &dataOut[sizeof(uint32_t)];
        int j = config.consoleH * (config.consoleW + 1);
        for (int i = 0; i < j - 1; ++i) {
            ascii[i] = 65 + i % 62;
        }
        ascii[j] = 0;
        //  TEMPORARY

        ((uint32_t *)dataOut)[0] = frameIndex;
        Sock::writeTo(consoleSock, dataOut, dataOutSize);

        msg = {Time::get(), frameIndex, Log::SrcPostASCII};
        //Sock::writeTo(logSock, &msg, sizeof(msg));

        usleep(1000000);
    }
    delete[] dataIn;
    delete[] dataOut;
}

//  Entry point
int main(int argc, char* argv[]) {
    Config::Instance config = Config::argsToConfig(argc, argv);

    pthread_t t;
    pthread_create(&t, NULL, pictureToASCII, (void *)&config);

    //  Terminate on ENTER
    std::cin.get();

    return 0;
}