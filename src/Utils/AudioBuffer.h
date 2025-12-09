#pragma once
#include <vector>

class AudioFloatBuffer {
public:
    AudioFloatBuffer(int numSamples, int numChannels)
        : samples(numSamples * numChannels),
          numSamples(numSamples),
          numChannels(numChannels) {}

    inline float* data() { return samples.data(); }
    inline const float* data() const { return samples.data(); }

    inline float& operator[](int i) { return samples[i]; }
    inline int size() const { return samples.size(); }
    inline void clear() {
        std::fill(samples.begin(), samples.end(), 0.0f);
    }

private:
    std::vector<float> samples;
    int numSamples;
    int numChannels;
};
