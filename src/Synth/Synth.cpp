#include "Synth.h"

void Synth::noteOn(float freq) {
    this->frequency = freq;
    this->phase = 0.0f;
    this->active = true;
}

void Synth::noteOff()
{
    this->active = false;
}

void Synth::renderAudio(int16_t* buffer, int numSamples) {

    if (!active) {
        for (int i = 0; i < numSamples * Constants::CHANNELS; i++) {
            buffer[i] = 0;
        }
        return;
    }

    const float phaseIncrement = (2.0f * M_PI * frequency) / Constants::SAMPLE_RATE;

    for (int i = 0; i < numSamples; i++) {
        float sample = sinf(phase);

        int16_t pcm = (int16_t) (sample * 20000.f);

        buffer[i * 2 + 0] = pcm;
        buffer[i * 2 + 1] = pcm;

        phase += phaseIncrement;
        if (phase >= 2.0f * M_PI)
            phase -= 2.0f * M_PI;
    }
}