#pragma once
#include <array>
#include <algorithm>
#include "../Utils/Constants.h"

class Step 
{
public:
    void setNote(const uint8_t& newNote);
    void setVelocity(const uint8_t& newVelocity);
    void setGate(const bool& newGate);
    uint8_t getNote() const { return note; }
    uint8_t getVelocity() const { return velocity; }
    bool getGate() const { return gate; }

private:
    uint8_t note = 60;
    bool gate = false;
    uint8_t velocity = 100;
};

// =======================================
// PATTERN
// =======================================

template <size_t MAX_STEPS>
class Pattern 
{
public:
    Pattern(size_t stepCount = 16)
        : stepCount(std::min(stepCount, MAX_STEPS))
    {
        for (auto& s : steps)
            s = Step{};
    }

    Step& getStep(size_t index) { return steps[index % stepCount]; }
    const Step& getStep(size_t index) const { return steps[index % stepCount]; }

    void setStepCount(size_t count) {
        stepCount = (count <= MAX_STEPS ? count : MAX_STEPS);
    }

    size_t getStepCount() const { return stepCount; }

private:
    size_t stepCount;
    std::array<Step, MAX_STEPS> steps;
};