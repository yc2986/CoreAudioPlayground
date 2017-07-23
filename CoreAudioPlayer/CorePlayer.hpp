//
//  CorePlayer.hpp
//  CoreAudioPlayer
//
//  Created by Yuzhou Cheng on 7/22/17.
//  Copyright © 2017 Yuzhou. All rights reserved.
//

#pragma once

#include <cstdio>
#include <thread>
#include <functional>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioToolBox/AudioToolBox.h>

namespace CoreAudioPlayer
{
    enum format_type {
        FMT_S16_LE,
        FMT_S16_BE,
        FMT_S32_LE,
        FMT_S32_BE,
        FMT_FLOAT
    };
    
    class CorePlayer
    {
    public:
        CorePlayer();
        virtual ~CorePlayer();
    public:     /* control api */
        bool Start(format_type format, int rate, int chan, AURenderCallbackStruct * callback);
        void Stop();
        void Pause(bool paused);
    public:     /* wave generator */
        void SineWave(const int iChannels, const float fFreq = 200.f, const float fDuration = 2.f);
    protected:
        AudioComponent m_AudioInstance;
        AudioComponentInstance m_AudioComponentInstance;
    };
}
