#include "Noise.h"

#include "utils/Logger.h"

Noise::Noise() {
    m_Handler.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    // m_Handler.SetFrequency(0.06f);
}
