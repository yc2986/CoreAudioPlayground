//
//  main.cpp
//  CoreAudioPlayer
//
//  Created by Yuzhou Cheng on 7/22/17.
//  Copyright © 2017 Yuzhou. All rights reserved.
//

#include <iostream>
#include "CorePlayer.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    CoreAudioPlayer::CorePlayer foo;
    foo.SineWave(4, 150.f, 10.f);
    return 0;
}
