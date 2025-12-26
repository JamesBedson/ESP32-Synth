#include "Synth.h"

Synth::Synth()
    : params(SynthParams::instance()),
      smoothedAmplitude(params.amplitude, Constants::SAMPLE_RATE, 0.02f, SmoothingMode::Linear),
      smoothedFreq(params.frequency, Constants::SAMPLE_RATE, 0.0001f, SmoothingMode::Exponential)
{
}

void Synth::registerParams(AudioParameterTree& tree)
{
    tree.add(&params.amplitude);
    tree.add(&params.frequency);
}

void Synth::noteOn(float freq)
{
    this->params.frequency.set(freq);
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

    const double phaseIncrement = (M_TWOPI * smoothedFreq.getRawValue()) / Constants::SAMPLE_RATE;

    for (int i = 0; i < numSamples; i++)
    {
        float sample = smoothedAmplitude.getNextValue() * sin(phase);

        for (int ch = 0; ch < numChannels; ch++)
        {
            buffer[i * numChannels + ch] = sample;
        }

        phase += phaseIncrement;
        if (phase >= M_TWOPI)
            phase -= M_TWOPI;
    }
}
