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
    Config::Instance &config = *(Config::Instance *)vargs;

    int cameraSock = Sock::create();
    Sock::clientConnect(cameraSock, config.clientIp, config.cameraPort);

    int logSock = Sock::create();
    Sock::clientConnect(logSock, config.serverIp, config.logPort);

    uint32_t frame = 0;
    uint64_t start = Time::get();

    Log::Message msg;
    uint64_t eta, delta;
    while (true)
    {
        Sock::writeTo(cameraSock, &frame, sizeof(frame));

        msg = {Time::get(), frame, Log::SrcClock};
        Sock::writeTo(logSock, &msg, sizeof(msg));

        eta = Time::diff(start, Time::get());
        delta = eta - frame * 1000000;

        ++frame;

        usleep(config.period - delta);
    }
}

//  Entry point
int main(int argc, char* argv[]) {
    Config::Instance config = Config::argsToConfig(argc, argv);

    pthread_t t;
    pthread_create(&t, NULL, clockThread, (void *)&config);

    //  Terminate on ENTER
    std::cin.get();

    return 0;
}