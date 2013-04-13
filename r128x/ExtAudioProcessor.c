// ExtAudioProcessor.c
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
// along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
// copyright Manuel Naudin 2012-2013

#define PROGRESS_NOTIFICATION CFSTR("R128X_Progress")
#define DEFAULT_BUFFER_SIZE 192000
#include "ExtAudioProcessor.h"
#include <math.h>

OSStatus ExtAudioConvCallback(AudioConverterRef inAudioConverter,
                         UInt32 *ioNumberDataPackets,
                         AudioBufferList *ioData,
                         AudioStreamPacketDescription **outDataPacketDescription,
                         void *inUserData)
{
    OSStatus err = noErr;
    
    //setting ExtAudioFile out AudioBufferList
    UInt32 framesInFileOutBuffer = *ioNumberDataPackets;
    AudioStreamBasicDescription converterInASBD;
    UInt32 propSize = sizeof(converterInASBD);
    err = AudioConverterGetProperty(inAudioConverter,
                                    kAudioConverterCurrentInputStreamDescription,
                                    &propSize,
                                    &converterInASBD);
    if (err != noErr)
        return err;
    ExtAudioData *userData = (ExtAudioData *)inUserData;
    Float32 *fileOutBuffer = userData->mFileOutBuffer;
    AudioBufferList fileOutBufferList;
    fileOutBufferList.mNumberBuffers = 1;
    fileOutBufferList.mBuffers[0].mNumberChannels = converterInASBD.mChannelsPerFrame;
    fileOutBufferList.mBuffers[0].mDataByteSize = framesInFileOutBuffer * converterInASBD.mBytesPerFrame;
    fileOutBufferList.mBuffers[0].mData = fileOutBuffer;
    
    //read audio data from file
    ExtAudioFileRef *audioFileRef = userData->mAudioFileRef;
    
    err = ExtAudioFileRead(*audioFileRef,
                           &framesInFileOutBuffer,
                           &fileOutBufferList);
    if (err != noErr)
        return err;
    
    userData->mFileFramesRead += framesInFileOutBuffer;
    UInt32 framesInBuffer = framesInFileOutBuffer;
    UInt32 pushedFrames = 0;
    Float32 *offset;
    double mom;
    while (framesInBuffer >= userData->mNeededFrames) {
        offset = (fileOutBuffer + (pushedFrames * converterInASBD.mChannelsPerFrame));
        ebur128_add_frames_float(userData->mState,
                                 offset,
                                 (size_t)userData->mNeededFrames);
        ebur128_loudness_momentary(userData->mState, &mom);
        CFNumberRef cmom = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &mom);
        CFArrayAppendValue(userData->mBlocks, cmom);
        framesInBuffer -= userData->mNeededFrames;
        pushedFrames += userData->mNeededFrames;
        userData->mNeededFrames = userData->mReportIntervalFrames;
    }
    if (framesInBuffer > 0) {
        offset = (fileOutBuffer + (pushedFrames * converterInASBD.mChannelsPerFrame));
        ebur128_add_frames_float(userData->mState,
                                 offset,
                                 (size_t)framesInBuffer);
        userData->mNeededFrames -= framesInBuffer;
    }
        
    float progress = 100.0 * userData->mFileFramesRead / userData->fileLengthInFrames;
    CFNumberRef num = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &progress);
    CFNotificationCenterPostNotification(CFNotificationCenterGetLocalCenter(),
                                         PROGRESS_NOTIFICATION,
                                         num,
                                         NULL, true);
    
    CFRelease(num);

    ioNumberDataPackets = &framesInFileOutBuffer;
    ioData->mBuffers[0].mData = fileOutBuffer;
    ioData->mBuffers[0].mDataByteSize = framesInFileOutBuffer * converterInASBD.mBytesPerFrame;
    return err;
}

OSStatus ExtAudioReader (CFStringRef audioFilePath,
                         double *il_p,
                         double *lra_p,
                         Float32 *maxtp_p)
{
    OSStatus err = noErr;
    CFURLRef myFileRef = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                                                       audioFilePath,
                                                       kCFURLPOSIXPathStyle,
                                                       false);
    ExtAudioFileRef audioFileRef;
    err = ExtAudioFileOpenURL(myFileRef, &audioFileRef);
    if (err != noErr)
        return err;
    CFRelease(myFileRef);
    
    //getting input file asbd
    AudioStreamBasicDescription inFileASBD;
    UInt32 propSize = sizeof(inFileASBD);
    err = ExtAudioFileGetProperty(audioFileRef,
                                  kExtAudioFileProperty_FileDataFormat,
                                  &propSize,
                                  &inFileASBD);
    if (err != noErr)
        return err;
    
    //setting ExtAudioFile client format
    AudioStreamBasicDescription clientASBD;
    clientASBD.mChannelsPerFrame = inFileASBD.mChannelsPerFrame;
    clientASBD.mSampleRate = inFileASBD.mSampleRate;
    clientASBD.mFormatID = kAudioFormatLinearPCM;
    clientASBD.mFormatFlags =
    kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    clientASBD.mBitsPerChannel = 32;
    clientASBD.mFramesPerPacket = 1;
    clientASBD.mBytesPerFrame = 4 * clientASBD.mChannelsPerFrame;
    clientASBD.mBytesPerPacket = clientASBD.mBytesPerFrame;
    propSize = sizeof(clientASBD);
    err = ExtAudioFileSetProperty(audioFileRef,
                                  kExtAudioFileProperty_ClientDataFormat,
                                  propSize,
                                  &clientASBD);
    if (err != noErr)
        return err;
    
    //setting AudioConverter in/out format
    AudioStreamBasicDescription converterInASBD = clientASBD;
    AudioStreamBasicDescription converterOutASBD = clientASBD;
    int overSamplingFactor;
    if (clientASBD.mSampleRate <= 48000) {
        overSamplingFactor = 4;
    } else if (clientASBD.mSampleRate <= 96000) {
        overSamplingFactor = 2;
    } else {
        overSamplingFactor = 1;
    }
    converterOutASBD.mSampleRate = overSamplingFactor * clientASBD.mSampleRate;
    AudioConverterRef converterRef;
    err = AudioConverterNew(&converterInASBD,
                            &converterOutASBD,
                            &converterRef);
    if (err != noErr)
        return err;
    
    // setting AudioConverter quality and oversampling complexity
    // disabled for now
    
     /*UInt32 converterQuality = kAudioConverterQuality_Max;
     propSize = sizeof(converterQuality);
     err = AudioConverterSetProperty(converterRef,
     kAudioConverterSampleRateConverterQuality,
     propSize,
     &converterQuality);
     assert(err == noErr);
     
    UInt32 converterComplexity = kAudioConverterSampleRateConverterComplexity_Mastering;
    propSize = sizeof(converterComplexity);
    err = AudioConverterSetProperty(converterRef,
                                    kAudioConverterSampleRateConverterComplexity,
                                    propSize,
                                    &converterComplexity);
    assert(err == noErr);
    if (err != noErr)
        return err;*/
    
    // setting AudioConverter out AudioBufferList
    UInt32 framesInConverterOutBuffer = overSamplingFactor * DEFAULT_BUFFER_SIZE;
    UInt8 *converterOutBuffer = (UInt8 *)malloc(framesInConverterOutBuffer * converterOutASBD.mBytesPerFrame);
    if (converterOutBuffer == NULL)
        return 1;
    AudioBufferList converterOutBufferList;
    converterOutBufferList.mNumberBuffers = 1;
    converterOutBufferList.mBuffers[0].mNumberChannels = converterOutASBD.mChannelsPerFrame;
    converterOutBufferList.mBuffers[0].mDataByteSize = framesInConverterOutBuffer * converterOutASBD.mBytesPerFrame;
    converterOutBufferList.mBuffers[0].mData = converterOutBuffer;
    
    //allocating intermediate buffer
    UInt32 converterInputBufferSize = converterOutBufferList.mBuffers[0].mDataByteSize;
    propSize = sizeof(UInt32);
    err = AudioConverterGetProperty(converterRef,
                                    kAudioConverterPropertyCalculateInputBufferSize,
                                    &propSize,
                                    &converterInputBufferSize);
    if (err != noErr)
        return err;

    Float32 *fileOutBuffer = (Float32 *)malloc(converterInputBufferSize);
    if (fileOutBuffer == NULL)
        return 1;
    
    //setting userData
    ExtAudioData userData = {0};
    userData.mAudioFileRef = &audioFileRef;
    ebur128_state *state = ebur128_init(clientASBD.mChannelsPerFrame,
                                        clientASBD.mSampleRate,
                                        EBUR128_MODE_I | EBUR128_MODE_LRA);
    if (state == NULL)
        return 1;
    userData.mState = state;
    userData.mFileOutBuffer = fileOutBuffer;
    userData.mFileFramesRead = 0;
    userData.mFramesProduced = 0;
    userData.mReportIntervalFrames = clientASBD.mSampleRate / 10;
    userData.mNeededFrames = userData.mReportIntervalFrames;
    UInt32 size = sizeof(SInt64);
    err = ExtAudioFileGetProperty(audioFileRef, kExtAudioFileProperty_FileLengthFrames, &size, &(userData.fileLengthInFrames));
    if (err != noErr)
        return err;
    Float32 maxTP = 0;
    
    userData.mBlocks = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    if (userData.mBlocks == NULL)
        return -1;
    
    //calling AudioConverterFillComplexBuffer
    UInt32 framesToRead;
    do
    {
        framesToRead = framesInConverterOutBuffer;
        err = AudioConverterFillComplexBuffer(converterRef,
                                              ExtAudioConvCallback,
                                              &userData,
                                              &framesToRead,
                                              &converterOutBufferList,
                                              nil);
        Float32 *samples = (Float32 *)converterOutBufferList.mBuffers[0].mData;
        UInt32 nChannels = converterOutBufferList.mBuffers[0].mNumberChannels;
        userData.mFramesProduced += framesToRead;
        for(int i = 0; i < framesToRead; i++)
        {
            for (int j = 0; j < nChannels; j++)
            {
                if (samples[(nChannels * i) + j] > maxTP)
                    maxTP = samples[(nChannels * i) + j];
            }
        }
        if (err != noErr && err != 'insz')
            return err;
        
    } while (framesToRead > 0 && (userData.mFileFramesRead < userData.fileLengthInFrames));
    
    double il, lra;
    ebur128_loudness_global(userData.mState, &il);
    il = rint(10 * il) / 10;
    *il_p = il;

    ebur128_loudness_range(userData.mState, &lra);
    *lra_p = lra;
    lra = rint(10 * lra) / 10;
    maxTP = rintf(10 * 20 * log10(maxTP)) / 10;
    *maxtp_p = maxTP;
    //CFIndex nblocks = CFArrayGetCount(userData.mBlocks);
    /*
    CFNumberRef value;
    double dvalue;
    Boolean ok;
    printf("nblocks : %ld\n", CFArrayGetCount(userData.mBlocks));
    for (long i = 0; i < CFArrayGetCount(userData.mBlocks); i++) {
        value = CFArrayGetValueAtIndex(userData.mBlocks, i);
        ok = CFNumberGetValue(value, kCFNumberDoubleType, &dvalue);
        printf("%ld %f\n", i, dvalue);
    }
     */
    /*
    char cstring[128];
    Boolean success;
    success = CFStringGetCString(audioFilePath, cstring, 128, kCFStringEncodingUTF8);
    printf("%s\t%.1f\t%.1f\t%.1f\n", cstring, il, lra, maxTP);
    */
    //cleaning
    CFRelease(userData.mBlocks);
    free(fileOutBuffer);
    ebur128_destroy(&state);
    free(converterOutBuffer);
    AudioConverterDispose(converterRef);
    ExtAudioFileDispose(audioFileRef);
    if (err == 'insz')
        err = noErr;
    return err;
}
