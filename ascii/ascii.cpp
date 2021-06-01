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
    size_t asciiSize = config.consoleH * (config.consoleW + 1);
    char *ascii = new char[asciiSize];
    //  Prepare img2txt arguments
    std::stringstream cmd;
    cmd << "./../img2txt -f temp.png -w " << config.consoleW << " -y " << (float)config.consoleH / (float)config.consoleW;
    //  Receive information about image
    int imgData[4];
    Sock::readFrom(cameraSock, imgData, sizeof(imgData));
    cv::Mat img(imgData[0], imgData[1], imgData[2]);
    size_t imgSize = imgData[0] * imgData[1] * imgData[3];
    //  Repeat until terminated
    while(true) {
        //  Receive frame and image
        Sock::readFrom(cameraSock, &frameIndex, sizeof(frameIndex));
        Sock::readFrom(cameraSock, img.data, imgSize);
        //  Log arrival time
        std::cout << "AAAAAAA" << frameIndex << std::endl;
        msg = {Time::get(), frameIndex, Log::SrcPreASCII};
        Sock::writeTo(logSock, &msg, sizeof(msg));
        //  Convert image to ASCII
        cv::imwrite("temp.png", img);
        strcpy(ascii, executeCmd(cmd.str()).c_str());
        //  Send converted image
        Sock::writeTo(consoleSock, &frameIndex, sizeof(frameIndex));
        Sock::writeTo(consoleSock, ascii, asciiSize);
        //  Log leave time
        std::cout << "BBB" << frameIndex << std::endl;
        msg = {Time::get(), frameIndex, Log::SrcPostASCII};
        Sock::writeTo(logSock, &msg, sizeof(msg));
    }
    delete[] ascii;
}

//  Entry point
int main(int argc, char* argv[]) {
    //  Pack arguments
    Config::Instance config = Config::argsToConfig(argc, argv);
    //  Create thread
    pthread_t t;
    pthread_create(&t, NULL, pictureToASCII, (void *)&config);
    //  Terminate on ENTER
    std::cin.get();
    return 0;
}