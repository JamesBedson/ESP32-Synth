#include "Synth.h"

void Synth::noteOn(float freq)
{
    this->frequency.set(freq);
    this->active = true;
}

void Synth::noteOff()
{
    this->active = false;
}

void Synth::renderAudio(float *buffer, int numSamples, int numChannels)
{

    if (!active)
    {
        return;
    }

    #ifndef M_TWOPI
    #define M_TWOPI (2.0f * M_PI)
    #endif

    const float phaseIncrement = (M_TWOPI * frequency.get()) / Constants::SAMPLE_RATE;

    for (int i = 0; i < numSamples; i++)
    {
        float sample = amplitude.get() * sinf(phase);

        for (int ch = 0; ch < numChannels; ch++)
        {
            buffer[i * numChannels + ch] = sample;
        }

        phase += phaseIncrement;
        if (phase >= M_TWOPI)
            phase -= M_TWOPI;
    }
}