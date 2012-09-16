//
//  fileprocessor.c
//  r128x
//
//  Created by manu naudin on 16/09/12.
//  Copyright (c) 2012 Manuel Naudin. All rights reserved.
//

#include "fileprocessor.h"
#include <math.h>

OSStatus CAConverterProc(AudioConverterRef inAudioConverter,
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
    UserData *userData = (UserData *)inUserData;
    UInt8 *fileOutBuffer = userData->mFileOutBuffer;
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
    ebur128_add_frames_float(userData->mState,
                             (float *)fileOutBuffer,
                             (size_t)framesInFileOutBuffer);
    
    ioNumberDataPackets = &framesInFileOutBuffer;
    ioData->mBuffers[0].mData = fileOutBuffer;
    ioData->mBuffers[0].mDataByteSize = framesInFileOutBuffer * converterInASBD.mBytesPerFrame;
    return err;
}

OSStatus CAReader (CFStringRef audioFilePath)
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
    UInt32 framesInConverterOutBuffer = overSamplingFactor*4096;
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

    UInt8 *fileOutBuffer = (UInt8 *)malloc(converterInputBufferSize);
    if (fileOutBuffer == NULL)
        return 1;
    
    //setting userData
    UserData userData = {0};
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
    
    Float32 maxTP = 0;
    
    //calling AudioConverterFillComplexBuffer
    UInt32 framesToRead;
    do
    {
        framesToRead = framesInConverterOutBuffer;
        err = AudioConverterFillComplexBuffer(converterRef,
                                              CAConverterProc,
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
        
    } while (framesToRead > 0);
    
    double il, lra;
    ebur128_loudness_global(userData.mState, &il);
    il = rint(10 * il) / 10;
    ebur128_loudness_range(userData.mState, &lra);
    lra = rint(10 * lra) / 10;
    maxTP = rintf(10 * 20 * log10(maxTP)) / 10;
    char cstring[128];
    Boolean success;
    success = CFStringGetCString(audioFilePath, cstring, 128, kCFStringEncodingUTF8);
    printf("%s\t%.1f\t%.1f\t%.1f\n", cstring, il, lra, maxTP);
    
    //cleaning
    free(fileOutBuffer);
    ebur128_destroy(&state);
    free(converterOutBuffer);
    AudioConverterDispose(converterRef);
    ExtAudioFileDispose(audioFileRef);
    if (err == 'insz')
        err = noErr;
    return err;
}
