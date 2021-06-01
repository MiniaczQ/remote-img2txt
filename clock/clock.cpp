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

//  Regurarly send signals to camera to take a photo
void * clockThread(void *vargs) {
    //  Unpack arguments
    Config::Instance &config = *(Config::Instance *)vargs;
    //  Connect sockets
    int cameraSock = Sock::fullConnectTo(config.clientIp, config.cameraPort);
    int logSock = Sock::fullConnectTo(config.serverIp, config.logPort);
    //  Allocate loop variables
    uint32_t frameIndex = 0;
    int64_t start = Time::get();
    Log::Message msg;
    //  Repeat until terminated
    while (true)
    {
        //  Send pulse to camera
        Sock::writeTo(cameraSock, &frameIndex, sizeof(frameIndex));
        //  Log
        msg = {Time::get(), frameIndex, Log::SrcClock};
        Sock::writeTo(logSock, &msg, sizeof(msg));
        //  Increment frame
        ++frameIndex;
        //  Sleep to send a signal every ~period microseconds
        usleep(frameIndex * config.period - Time::diff(start, Time::get()));
    }
}

//  Entry point
int main(int argc, char* argv[]) {
    //  Initialize config
    Config::Instance config = Config::argsToConfig(argc, argv);
    //  Pack arguments and start a thread
    pthread_t t;
    pthread_create(&t, NULL, clockThread, (void *)&config);
    //  Terminate on ENTER
    std::cin.get();
    return 0;
}