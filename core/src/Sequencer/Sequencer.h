#pragma once
#include <stdint.h>

class Sequencer
{
public:
    Sequencer(float bpm = 120.0f);
    void setBPM(const float& newBPM);

    void start() { running = true; }
    void stop() { running = false; }
    void reset();

    bool tick(float elapsedMs); 

    uint8_t getCurrentStep() const { return currentStep; } 

private:

    float bpm;
    float stepDurationMs;
    float accumulatorMs = 0;

    uint8_t currentStep = 0;
    bool running = false;
};