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
void * clock(void * vargs) {
    Config &cfg = *(Config *)vargs;

    int cameraSock = sock::create();
    sock::clientConnect(cameraSock, cfg.clientIp, cfg.cameraPort);

    int logSock = sock::create();
    sock::clientConnect(logSock, cfg.serverIp, cfg.logPort);

    uint32_t frame = 0;
    uint64_t start = Time::get();

    while(true) {
        write(cameraSock, &frame, sizeof(frame));

        LogMsg msg{Time::get(), frame, SrcClock};
        write(logSock, &msg, sizeof(msg));

        uint64_t eta = Time::diff(start, Time::get());
        uint64_t delta = eta - frame*1000000;

        ++frame;

        usleep(cfg.period - delta);
    }
}

//  Entry point
int main(int argc, char* argv[]) {
    Config cfg = getConfig(argc, argv);

    pthread_t t;
    pthread_create(&t, NULL, clock, (void *)&cfg);

    //  Terminate on ENTER
    std::cin.get();

    return 0;
}