#ifndef INCLUDED_TIMER
#define INCLUDED_TIMER

#include <chrono>

namespace range2 {

// timer taken from
// https://github.com/psoberoi/stepanov-conversations-course/blob/master/tape/timer.h
//
class timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    double stop() {
        auto stop_time = std::chrono::high_resolution_clock::now();
        return double(std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count());
    }
};

} // namespace range2

#endif
