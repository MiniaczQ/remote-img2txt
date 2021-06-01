#include <iostream>
#include <string>
#include <cstdint>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sstream>

#include <opencv2/opencv.hpp>

#include "../libs/config.cpp"
#include "../libs/log.cpp"
#include "../libs/socket.cpp"
#include "../libs/time.cpp"

//  Executes a shell command, stores the result in buffer
std::string executeCmd(std::string cmd) {
    //  Execute command
    FILE *cmdout = popen(cmd.c_str(), "r");
    if (!cmdout) {
        std::cout << "Failed to open pipe." << std::endl;
        throw -1;
    }
    //  Read output
    char buffer[128];
    std::string result;
    while(feof(cmdout) == 0)
        if (fgets(buffer, 128, cmdout) != nullptr)
            result += buffer;
    //  Cleanup and return
    pclose(cmdout);
    return result;
}

//  Turn image to ASCII
void * pictureToASCII(void *vargs) {
    //  Unpack arguments
    Config::Instance &config = *(Config::Instance *)vargs;
    //  Connect sockets
    int cameraSock = Sock::fullHostOne(config.serverIp, config.asciiPort);
    int consoleSock = Sock::fullConnectTo(config.clientIp, config.consolePort);
    int logSock = Sock::fullConnectTo(config.serverIp, config.logPort);
    //  Allocate loop variables
    uint32_t frameIndex;
    Log::Message msg;
    //  Prepare img2txt arguments
    std::stringstream cmd;
    cmd << "./../img2txt -f temp.png -w " << config.consoleW << " -y " << (float)config.consoleH / (float)config.consoleW << " -m " << config.consoleP;
    //  Receive information about image
    int imgData[4];
    Sock::readFrom(cameraSock, imgData, sizeof(imgData));
    cv::Mat img(imgData[0], imgData[1], imgData[2]);
    //  Allocate input buffer
    size_t inDataSize = sizeof(frameIndex) + imgData[0] * imgData[1] * imgData[3];
    uint8_t *inData = new uint8_t[inDataSize];
    //  Allocate output buffer
    size_t outDataSize = sizeof(frameIndex) + config.consoleH * (config.consoleW + 1);
    char *outData = new char[outDataSize];
    //  Repeat until terminated
    while(true) {
        //  Receive frame and image
        Sock::readFrom(cameraSock, inData, inDataSize);
        frameIndex = ((uint32_t *)inData)[0];
        //  Log arrival time
        msg = {Time::get(), frameIndex, Log::SrcPreASCII};
        Sock::writeTo(logSock, &msg, sizeof(msg));
        //  Convert image to ASCII
        memcpy(img.ptr<char>(0), &inData[sizeof(frameIndex)], inDataSize - sizeof(frameIndex));
        cv::imwrite("temp.png", img);
        strcpy(&outData[sizeof(frameIndex)], executeCmd(cmd.str()).c_str());
        //  Send converted image
        ((uint32_t *)outData)[0] = frameIndex;
        Sock::writeTo(consoleSock, outData, outDataSize);
        //  Log leave time
        msg = {Time::get(), frameIndex, Log::SrcPostASCII};
        Sock::writeTo(logSock, &msg, sizeof(msg));
    }
    delete[] inData;
    delete[] outData;
}

//  Entry point
int main(int argc, char* argv[]) {
    //  Initialize config
    Config::Instance config = Config::argsToConfig(argc, argv);
    //  Pack arguments and start a thread
    pthread_t t;
    pthread_create(&t, NULL, pictureToASCII, (void *)&config);
    //  Terminate on ENTER
    std::cin.get();
    return 0;
}