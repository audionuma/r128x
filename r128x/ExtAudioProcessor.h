// ExtAudioProcessor.h
// This file is part of r128x.
//
// r128x is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// r128x is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with r128x.  If not, see <http://www.gnu.org/licenses/>.
// copyright Manuel Naudin 2012-2013 

#ifndef r128x_fileprocessor_h
#define r128x_fileprocessor_h

#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include "ebur128.h"

// a struct to keep some user data
typedef struct ExtAudioData
{
    ExtAudioFileRef *mAudioFileRef;
    Float32 *mFileOutBuffer;
    ebur128_state *mState;
    
    UInt32 mFileFramesRead;
    UInt32 mFramesProduced;
    UInt32 mNeededFrames;
    UInt32 mReportIntervalFrames; //experimental block logging every interval
    SInt64 fileLengthInFrames;
    
    CFMutableArrayRef mBlocks; //to store momentary blocks
    
    
} ExtAudioData;

// a callback for an ExtAudioReader that uses ExtAudioFileRead to get samples from a file
OSStatus ExtAudioConvCallback(AudioConverterRef inAudioConverter,
                         UInt32 *ioNumberDataPackets,
                         AudioBufferList *ioData,
                         AudioStreamPacketDescription **outDataPacketDescription,
                         void *inUserData);

// a function that will measure r128 descriptors from a file.
// using ExtAudioConvCallback as callback
OSStatus ExtAudioReader (CFStringRef audioFilePath,
                         double *il_p,
                         double *lra_p,
                         Float32 *maxtp_p);

#endif
