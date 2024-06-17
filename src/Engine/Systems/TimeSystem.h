#pragma once

#include <chrono>
#include <thread>

class TimeSystem {
public:
    TimeSystem(int targetFPS)
            : targetFPS(targetFPS),
              frameDuration(1000 / targetFPS) {}

    void startFrame()
    {
        frameStart = std::chrono::high_resolution_clock::now();
    }

    void endFrame()
    {
        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
        sleepForRemainingFrameTime(elapsedTime);
    }

    void sleepFor(std::chrono::milliseconds duration)
    {
        std::this_thread::sleep_for(duration);
    }

private:
    int targetFPS;
    std::chrono::milliseconds frameDuration;
    std::chrono::high_resolution_clock::time_point frameStart;

    void sleepForRemainingFrameTime(std::chrono::milliseconds elapsedTime) {
        if (elapsedTime < frameDuration) {
            std::this_thread::sleep_for(frameDuration - elapsedTime);
        }
    }
};