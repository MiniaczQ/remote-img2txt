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

void * pictureToASCII(void * vargs) {
    Config &cfg = *(Config *)vargs;
    int sender = sock::create();
    sock::clientConnect(sender, cfg.serverIp, cfg.logPort);

    uint16_t frame = 0;

    while(true) {
        LogMsg msg{0, frame, SrcASCII};
        write(sender, &msg, sizeof(msg));
        ++frame;
        usleep(1000000);
    }
}

//  Entry point
int main(int argc, char* argv[]) {
    Config cfg = getConfig(argc, argv);

    pthread_t t;
    pthread_create(&t, NULL, pictureToASCII, (void *)&cfg);

    //  Terminate on ENTER
    std::cin.get();

    return 0;
}