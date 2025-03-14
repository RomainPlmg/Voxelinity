#ifndef __NOISE_H__
#define __NOISE_H__

#include <FastNoiseLite.h>

#include <vector>

class Noise {
   public:
    Noise();

    /* Getters */
    const std::vector<float>& GetNoiseData() const { return m_NoiseData; }

   private:
    FastNoiseLite m_Handler;
    std::vector<float> m_NoiseData;
};

#endif  // __NOISE_H__