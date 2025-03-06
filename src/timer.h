#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;

public:
    // Start the timer
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    // Get elapsed time in milliseconds (combines stop and get time)
    double getElapsedTime() {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(endTime - startTime).count();
    }
};

#endif // TIMER_H

