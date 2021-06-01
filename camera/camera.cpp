#include <iostream>
#include <string>
#include <cstdint>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include <opencv2/opencv.hpp>

#include "../libs/config.cpp"
#include "../libs/log.cpp"
#include "../libs/socket.cpp"
#include "../libs/time.cpp"

//  Capture a picture
void *cameraThread(void *vargs) {
    //  Unpack arguments
    Config::Instance &config = *(Config::Instance *)vargs;
    //  Connect sockets
    int clockSock = Sock::fullHostOne(config.clientIp, config.cameraPort);
    int asciiSock = Sock::fullConnectTo(config.serverIp, config.asciiPort);
    int logSock = Sock::fullConnectTo(config.serverIp, config.logPort);
    //  Allocate loop variables
    uint32_t frameIndex;
    Log::Message msg;
    cv::Mat img;
    cv::VideoCapture cam("../ascii/test.mp4");
    //  Send information about image
    cam.read(img);
    int imgData[4];
    imgData[0] = img.rows;
    imgData[1] = img.cols;
    imgData[2] = img.type();
    imgData[3] = img.channels();
    Sock::writeTo(asciiSock, imgData, sizeof(imgData));
    //  Repeat until terminated
    while(true) {
        //  Wait for clock signal
        Sock::readFrom(clockSock, &frameIndex, sizeof(frameIndex));
        //  Send picture to ASCII
        cam.read(img);
        Sock::writeTo(asciiSock, &frameIndex, sizeof(frameIndex));
        Sock::writeTo(asciiSock, img.ptr<char>(0), img.dataend - img.datastart);
        std::cout << "size " << (int)(img.dataend - img.datastart) << std::endl;
        //  Send log data
        std::cout << frameIndex << std::endl;
        msg = {Time::get(), frameIndex, Log::SrcCamera};
        Sock::writeTo(logSock, &msg, sizeof(msg));
    }
}

//  Entry point
int main(int argc, char *argv[]) {
    Config::Instance config = Config::argsToConfig(argc, argv);

    pthread_t t;
    pthread_create(&t, NULL, cameraThread, (void *)&config);

    //  Terminate on ENTER
    std::cin.get();

    return 0;
}