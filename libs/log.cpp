#include <chrono>
#include <cstdint>
#include <semaphore.h>
#include <string.h>

#define MAX_LOGS 64
#define LOGS_PATH "logs.txt"

//  Possible message sources + their count
enum SourceTypes
{
    //SrcCamera,
    SrcASCII,
    //SrcConsole,
    SrcsCount
};

//  Message passed between processes
typedef struct {
    uint64_t ticks;
    uint32_t frameIndex;
    uint8_t sourceType;
} LogMsg;

//  Single log entry, gathers data with the same frame index
typedef struct {
    uint64_t ticks[SrcsCount];
    uint32_t index;
    uint8_t gathered_sources;
} LogFrame;

//  Store and synchronize logs, dump to logs file when ready
class LogBuffer {
private:
    LogFrame buffer[MAX_LOGS];
    sem_t mutex;
    std::ofstream logsFile;

public:
    //  Empty the buffer, initiate mutex, open logs file
    LogBuffer() {
        memset(buffer, 0, sizeof(buffer));
        sem_init(&mutex, 0, 1);
        logsFile = std::ofstream(LOGS_PATH);
    }

    //  Destroy mutex, close logs file
    ~LogBuffer() {
        sem_destroy(&mutex);
        logsFile.close();
    }

    //  Add message to the buffer
    void addMsg(LogMsg msg) {
        sem_wait(&mutex);

        uint32_t i = msg.frameIndex % MAX_LOGS;
        LogFrame &frame = buffer[i];

        if (msg.frameIndex < frame.index) {
            //  Old message, buffer already overwrote entries for this frame, hence ignore
        } else {
            if (msg.frameIndex > frame.index) {
                //  New message, overwrite previous message
                memset(&frame, 0, sizeof(LogFrame));
                frame.index = msg.frameIndex;
            }

            //  Current message, add contents to buffer
            frame.ticks[msg.sourceType] = msg.ticks;
            ++frame.gathered_sources;

            if (frame.gathered_sources == SrcsCount) {
                //  Entry full, dump to logs file
                logsFile << "[" << frame.index << "]"
                         << std::endl;
            }
        }

        sem_post(&mutex);
    }
};