#include "PlaybackEngine.h"

PlaybackEngine::PlaybackEngine(
    Sequencer& sequencer,
    Pattern<Constants::MAX_STEP_SIZE>& pattern,
    Synth& synth
) : 
sequencer(sequencer), 
pattern(pattern), 
synth(synth)
{
}

void PlaybackEngine::update(float elapsedMs) {
    
    if (sequencer.tick(elapsedMs)) {
        size_t stepIdx = sequencer.getCurrentStep();
        Step& step = pattern.getStep(stepIdx);

        if (step.getGate()) {
            float freq = MidiUtils::midiToFreq(step.getNote());
            synth.noteOn(freq);
        } 

        else {
            synth.noteOff();
        }
    }   
}