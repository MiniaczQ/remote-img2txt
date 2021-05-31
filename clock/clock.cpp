#include <iostream>
#include <string>
#include <cstdint>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "../libs/config.cpp"
#include "../libs/log.cpp"
#include "../libs/socket.cpp"

void * clock(void * vargs) {
    Config &cfg = *(Config *)vargs;
    int cameraSock = sock::create();
    sock::clientConnect(cameraSock, cfg.clientIp, cfg.cameraPort);

    uint32_t frame = 0;

    while(true) {
        write(cameraSock, &frame, sizeof(frame));
        ++frame;
        usleep(cfg.period);
    }
}

int main(int argc, char* argv[]) {
    Config cfg = getConfig(argc, argv);

    pthread_t t;
    pthread_create(&t, NULL, clock, (void *)&cfg);

    //  Terminate on ENTER
    std::cin.get();

    return 0;
}