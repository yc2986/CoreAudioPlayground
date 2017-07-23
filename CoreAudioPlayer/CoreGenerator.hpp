//
//  CoreGenerator.hpp
//  CoreAudioPlayer
//
//  Created by Yuzhou Cheng on 7/23/17.
//  Copyright © 2017 Yuzhou. All rights reserved.
//

#pragma once

#include <vector>

namespace CoreAudioPlayer
{
    std::vector<int16_t> Sine(const float fSampleRate, const float fFreqs, const float fDuration = 2.f, const float fAmp = 0.5f);
    std::vector<int16_t> Interleave(const std::vector<std::vector<int16_t>> &vvData);
}
