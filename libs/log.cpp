#include <chrono>
#include <cstdint>

enum SourceTypes
{
    SrcCamera = 0b00000001,
    SrcASCII = 0b00000010,
    SrcConsole = 0b00000100
};

typedef struct {
    uint32_t frameId;
    uint8_t sourceType;
    //  Timestamp
} LogEntry;