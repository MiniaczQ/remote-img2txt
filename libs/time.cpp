#include <cstdint>
#include <chrono>

//  For time related things
namespace Time {
    using Clock = std::chrono::system_clock;
    //  Returns a serializable system time value
    uint64_t get() {
        return Clock::now().time_since_epoch().count();
    }

    //  Returns the difference in microseconds between 2 serializable system time values
    uint64_t diff(uint64_t a, uint64_t b) {
        Clock::duration delta = Clock::time_point(Clock::duration(b)) - Clock::time_point(Clock::duration(a));
        return (std::chrono::duration_cast<std::chrono::microseconds>(delta)).count();
    }
}