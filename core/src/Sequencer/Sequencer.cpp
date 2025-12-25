#include "Sequencer.h"

Sequencer::Sequencer(float bpm) {
    setBPM(bpm);
}

void Sequencer::setBPM(const float& newBPM) {
    bpm = newBPM;
    stepDurationMs = 60000.0f / bpm;
}

void Sequencer::reset() {
    currentStep = 0;
    accumulatorMs = 0;
}

bool Sequencer::tick(float elapsedMs) {
    if (!running)
            return false;

        accumulatorMs += elapsedMs;
 
        if (accumulatorMs >= stepDurationMs) {
            accumulatorMs -= stepDurationMs;
            currentStep++;
            return true;
        }

        return false;
}