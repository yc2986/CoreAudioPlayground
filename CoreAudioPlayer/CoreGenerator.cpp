//
//  CoreGenerator.cpp
//  CoreAudioPlayer
//
//  Created by Yuzhou Cheng on 7/23/17.
//  Copyright © 2017 Yuzhou. All rights reserved.
//

#include "CoreGenerator.hpp"

#include <cmath>
#include <algorithm>

namespace CoreAudioPlayer
{
    std::vector<int16_t> Sine(const float fSampleRate, const float fFreqs, const float fDuration, const float fAmp /* = 0.5f */)
    {
        long lTotalFrames = static_cast<long>(fSampleRate * fDuration);
        std::vector<int16_t> vSineWave(lTotalFrames, 0);
        for (long frame = 0; frame < lTotalFrames; frame++)
            vSineWave[frame] = static_cast<int16_t>(fAmp * std::abs((float)SHRT_MIN) * std::sin(2 * M_PI * fFreqs * frame / fSampleRate));
        return vSineWave;
    }
    
    std::vector<int16_t> Interleave(const std::vector<std::vector<int16_t>> &vvData)
    {
        auto numChannels = vvData.size();
        auto numFrames = vvData.front().size();
        long lTotalFrames = numChannels * numFrames;
        std::vector<int16_t> vInterleave(lTotalFrames, 0);
        for (long frame = 0; frame < numFrames; frame++)
        {
            auto shift = frame * numChannels;
            for (int ch = 0; ch < numChannels; ch++)
                vInterleave[shift + ch] = vvData[ch][frame];
        }
        return vInterleave;
    }
}
