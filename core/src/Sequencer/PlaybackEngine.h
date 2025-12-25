#pragma once
#include "Sequencer.h"
#include "Pattern.h"
#include "../Utils/Constants.h"
#include "../Utils/MidiUtils.h"
#include "../Synth/Synth.h"

class PlaybackEngine
{
public:
    PlaybackEngine(
        Sequencer& sequencer,
        Pattern<Constants::MAX_STEP_SIZE>& pattern,
        Synth& synth
    );

    void update(float elapsedMs);

private:

    Sequencer& sequencer;
    Pattern<Constants::MAX_STEP_SIZE>& pattern;
    Synth& synth;
};