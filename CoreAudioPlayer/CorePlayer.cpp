//
//  CorePlayer.cpp
//  CoreAudioPlayer
//
//  Created by Yuzhou Cheng on 7/22/17.
//  Copyright © 2017 Yuzhou. All rights reserved.
//

#include "CorePlayer.hpp"
#include "CoreGenerator.hpp"

#include <cmath>
#include <climits>
#include <cassert>
#include <string>

namespace CoreAudioPlayer
{
    struct CoreAudioFormatDescriptionMap {
        enum format_type type;
        int mBitsPerChannel;
        int buffer_bytes_per_channel;
        unsigned int mFormatFlags;
    };
    static struct CoreAudioFormatDescriptionMap format_map[] = {
        {FMT_S16_LE, 16, sizeof(int16_t), kAudioFormatFlagIsSignedInteger},
        {FMT_S16_BE, 16, sizeof(int16_t), kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian},
        {FMT_S32_LE, 32, sizeof(int32_t), kAudioFormatFlagIsSignedInteger},
        {FMT_S32_BE, 32, sizeof(int32_t), kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian},
        {FMT_FLOAT,  32, sizeof(float),   kAudioFormatFlagIsFloat},
    };
    
    CorePlayer::CorePlayer()
    {
        /* open the default audio device */
        AudioComponentDescription desc;
        desc.componentType = kAudioUnitType_Output;
        desc.componentSubType = kAudioUnitSubType_DefaultOutput;
        desc.componentFlags = 0;
        desc.componentFlagsMask = 0;
        desc.componentManufacturer = kAudioUnitManufacturer_Apple;
        
        m_AudioInstance = AudioComponentFindNext(nullptr, &desc);
        if (! m_AudioInstance)
        {
            fprintf (stderr, "Failed to open default audio device.\n");
            throw;
        }
        
        if (AudioComponentInstanceNew(m_AudioInstance, &m_AudioComponentInstance))
        {
            fprintf (stderr, "Failed to open default audio device.\n");
            throw;
        }
    }
    
    CorePlayer::~CorePlayer()
    {
        AudioComponentInstanceDispose(m_AudioComponentInstance);
    }
    
    bool CorePlayer::Start(format_type format, int rate, int chan, AURenderCallbackStruct * callback)
    {
        struct CoreAudioFormatDescriptionMap * m = nullptr;
        
        for (struct CoreAudioFormatDescriptionMap it : format_map)
        {
            if (it.type == format)
            {
                m = & it;
                break;
            }
        }
        
        if (! m)
        {
            fprintf (stderr, "The requested audio format %d is unsupported.\n", format);
            return false;
        }
        
        if (AudioUnitInitialize (m_AudioComponentInstance))
        {
            fprintf (stderr, "Unable to initialize audio unit instance\n");
            return false;
        }
        
        AudioStreamBasicDescription streamFormat;
        streamFormat.mSampleRate = rate;
        streamFormat.mFormatID = kAudioFormatLinearPCM;
        streamFormat.mFormatFlags = m->mFormatFlags;
        streamFormat.mFramesPerPacket = 1;
        streamFormat.mChannelsPerFrame = chan;
        streamFormat.mBitsPerChannel = m->mBitsPerChannel;
        streamFormat.mBytesPerPacket = chan * m->buffer_bytes_per_channel;
        streamFormat.mBytesPerFrame = chan * m->buffer_bytes_per_channel;
        
        printf ("Stream format:\n");
        printf (" Channels: %d\n", streamFormat.mChannelsPerFrame);
        printf (" Sample rate: %f\n", streamFormat.mSampleRate);
        printf (" Bits per channel: %d\n", streamFormat.mBitsPerChannel);
        printf (" Bytes per frame: %d\n", streamFormat.mBytesPerFrame);
        
        if (AudioUnitSetProperty (m_AudioComponentInstance, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamFormat, sizeof(streamFormat)))
        {
            fprintf (stderr, "Failed to set audio unit input property.\n");
            return false;
        }
        
        if (AudioUnitSetProperty (m_AudioComponentInstance, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, callback, sizeof (AURenderCallbackStruct)))
        {
            fprintf (stderr, "Unable to attach an IOProc to the selected audio unit.\n");
            return false;
        }
        
        if (AudioOutputUnitStart (m_AudioComponentInstance))
        {
            fprintf (stderr, "Unable to start audio unit.\n");
            return false;
        }
        
        return true;
    }
    
    void CorePlayer::Stop()
    {
        AudioOutputUnitStop(m_AudioComponentInstance);
    }
    
    void CorePlayer::Pause(bool paused)
    {
        if (paused)
            AudioOutputUnitStop (m_AudioComponentInstance);
        else
        {
            if (AudioOutputUnitStart (m_AudioComponentInstance))
            {
                fprintf (stderr, "Unable to restart audio unit after pausing.\n");
                Stop();
            }
        }
    }
    
    void CorePlayer::SineWave(const int iChannels, const float fFreq /* = 200.f */, const float fDuration /* = 2.f */)
    {
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFURLRef resourcesURL = CFBundleCopyBundleURL(mainBundle);
        CFURLRef fullURL = CFURLCreateCopyAppendingPathComponent(kCFAllocatorDefault, resourcesURL, CFSTR("sine.wav"), false);
        CFRelease(resourcesURL);
        
        AudioStreamBasicDescription desc = {};
        desc.mSampleRate = 48000.f;
        desc.mFormatID = kAudioFormatLinearPCM;
        desc.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
        desc.mBitsPerChannel = 8 * sizeof(int16_t);  // 16 bits
        desc.mChannelsPerFrame = iChannels;
        desc.mFramesPerPacket = 1;
        desc.mBytesPerFrame = desc.mChannelsPerFrame * desc.mBitsPerChannel / 8;
        desc.mBytesPerPacket = desc.mFramesPerPacket * desc.mBytesPerFrame;
        
        /* craft file */
        AudioFileID fAudio;
        
        auto err = AudioFileCreateWithURL(
                      fullURL,
                      kAudioFileWAVEType,
                      &desc,
                      kAudioFileFlags_EraseFile,
                      &fAudio
                    );
        assert(err == noErr);
        
        /* craft sine waves */
        std::vector<std::vector<int16_t>> vvSineWave;
        for (int ch = 0; ch < iChannels; ch++)
            vvSineWave.push_back(Sine(desc.mSampleRate, fFreq * (ch+1), fDuration));
        
        auto vSineWave = Interleave(vvSineWave);
        
        UInt32 bytesToWrite = sizeof(int16_t) * vSineWave.size();
        err = AudioFileWriteBytes(fAudio, false, 0, &bytesToWrite, vSineWave.data());
        assert(err == noErr);
        
        err = AudioFileClose(fAudio);
        assert(err == noErr);
    }
}
