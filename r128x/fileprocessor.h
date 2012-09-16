//
//  fileprocessor.h
//  r128x
//
//  Created by manu naudin on 16/09/12.
//  Copyright (c) 2012 Manuel Naudin. All rights reserved.
//

#ifndef r128x_fileprocessor_h
#define r128x_fileprocessor_h

#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include "ebur128.h"

// a struct to keep some user data
typedef struct UserData
{
    ExtAudioFileRef *mAudioFileRef;
    UInt8 *mFileOutBuffer;
    ebur128_state *mState;
    
    UInt32 mFileFramesRead;
    UInt32 mFramesProduced;
    
} UserData;

// a callback for an AudioConberter that uses ExtAudioFileRead to get samples from a file
OSStatus CAConverterProc(AudioConverterRef inAudioConverter,
                         UInt32 *ioNumberDataPackets,
                         AudioBufferList *ioData,
                         AudioStreamPacketDescription **outDataPacketDescription,
                         void *inUserData);

// a function that will measure r128 descriptors from a file.
// using CAConverterCallback as callback
OSStatus CAReader (CFStringRef audioFilePath);

#endif
