#include <cstdint>
#include <chrono>

#ifndef MY_TIME
#define MY_TIME

//  For time related things
namespace Time {
    using Clock = std::chrono::system_clock;
    
    //  Returns a serializable system time value
    int64_t get() {
        return Clock::now().time_since_epoch().count();
    }

    //  Returns the difference in microseconds between 2 serializable system time values
    int64_t diff(int64_t a, int64_t b) {
        Clock::duration delta = Clock::time_point(Clock::duration(b)) - Clock::time_point(Clock::duration(a));
        return (std::chrono::duration_cast<std::chrono::microseconds>(delta)).count();
    }

    //  Return localtime since some unknown moment in microseconds
    int64_t microsec(int64_t a) {
        return (std::chrono::duration_cast<std::chrono::microseconds>(Clock::duration(a))).count();
    }
}
#endif