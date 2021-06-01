#include <chrono>
#include <cstdint>
#include <semaphore.h>
#include <string.h>

#include "../libs/time.cpp"

#define MAX_LOGS 64
#define LOGS_PATH "logs.txt"

namespace Log {
    //  Possible message sources + their count
    enum SourceTypes
    {
        SrcClock,
        SrcPreCamera,
        SrcPostCamera,
        SrcPreASCII,
        SrcPostASCII,
        SrcConsole,
        SrcsCount
    };

    //  Message passed between processes
    typedef struct {
        uint64_t ticks;
        uint32_t frameIndex;
        uint8_t sourceType;
    } Message;

    //  Single log entry, gathers data with the same frame index
    typedef struct {
        uint64_t ticks[SrcsCount];
        uint32_t index;
        uint8_t gathered_sources;
    } Frame;

    //  Store and synchronize logs, dump to logs file when ready
    class Buffer {
    private:
        Frame buffer[MAX_LOGS];
        sem_t mutex;
        std::ofstream logsFile;

    public:
        //  Empty the buffer, initiate mutex, open logs file
        Buffer() {
            memset(buffer, 0, sizeof(buffer));
            sem_init(&mutex, 0, 1);
            logsFile = std::ofstream(LOGS_PATH);
        }

        //  Destroy mutex, close logs file
        ~Buffer() {
            sem_destroy(&mutex);
            logsFile.close();
        }

        //  Store 
        void dumpFrame(Frame &frame) {
            logsFile << frame.index << ";"
                     << (float)Time::diff(frame.ticks[Log::SrcClock], frame.ticks[Log::SrcPreASCII]) / 1000000 << ";"
                     << (float)Time::diff(frame.ticks[Log::SrcPreASCII], frame.ticks[Log::SrcPostASCII]) / 1000000 << ";"
                     << (float)Time::diff(frame.ticks[Log::SrcPreCamera], frame.ticks[Log::SrcPostCamera]) / 1000000 << ";"
                     << (float)Time::diff(frame.ticks[Log::SrcPostCamera], frame.ticks[Log::SrcConsole]) / 1000000 << ";"
                     << std::endl;
        }

        //  Add message to the buffer
        void addMsg(Message msg) {
            sem_wait(&mutex);

            uint32_t i = msg.frameIndex % MAX_LOGS;
            Frame &frame = buffer[i];

            if (msg.frameIndex < frame.index) {
                //  Old message, buffer already overwrote entries for this frame, hence ignore
            } else {
                if (msg.frameIndex > frame.index) {
                    //  New message, overwrite previous message
                    memset(&frame, 0, sizeof(Frame));
                    frame.index = msg.frameIndex;
                }

                //  Current message, add contents to buffer
                frame.ticks[msg.sourceType] = msg.ticks;
                ++frame.gathered_sources;

                if (frame.gathered_sources == SrcsCount) {
                    dumpFrame(frame);
                }
            }

            sem_post(&mutex);
        }
    };
}