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
    cv::VideoCapture cam("./test.mp4");
    //  Send information about image
    cam.read(img);
    int imgData[4];
    imgData[0] = img.rows;
    imgData[1] = img.cols;
    imgData[2] = img.type();
    imgData[3] = img.channels();
    Sock::writeTo(asciiSock, imgData, sizeof(imgData));
    //  Allocate output buffer
    size_t outDataSize = sizeof(frameIndex) + imgData[0] * imgData[1] * imgData[3];
    uint8_t *outData = new uint8_t[outDataSize];
    //  Repeat until terminated
    while(true) {
        //  Wait for clock signal
        Sock::readFrom(clockSock, &frameIndex, sizeof(frameIndex));
        //  Get picture
        cam.read(img);
        //  Send picture
        ((uint32_t *)outData)[0] = frameIndex;
        memcpy(&(outData[sizeof(frameIndex)]), img.ptr<char>(0), outDataSize - sizeof(frameIndex));
        Sock::writeTo(asciiSock, outData, outDataSize);
        //  Send log data
        msg = {Time::get(), frameIndex, Log::SrcCamera};
        Sock::writeTo(logSock, &msg, sizeof(msg));
    }
    delete[] outData;
}

//  Entry point
int main(int argc, char *argv[]) {
    //  Initialize config
    Config::Instance config = Config::argsToConfig(argc, argv);
    //  Pack arguments and start a thread
    pthread_t t;
    pthread_create(&t, NULL, cameraThread, (void *)&config);
    //  Terminate on ENTER
    std::cin.get();
    return 0;
}