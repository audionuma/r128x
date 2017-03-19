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
// along with r128x.  If not, see <http://www.gnu.org/licenses/>.
// copyright Manuel Naudin 2012-2013

#define PROGRESS_NOTIFICATION CFSTR("R128X_Progress")
#define DEFAULT_FRAMES_IN_BUFFER 192000
#include "ExtAudioProcessor.h"
#include <math.h>


OSStatus ExtAudioReader (CFStringRef audioFilePath,
                         double *il_p,
                         double *lra_p,
                         double *maxtp_p)
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
  
  // getting input file asbd
  AudioStreamBasicDescription inFileASBD;
  UInt32 propSize = sizeof(inFileASBD);
  err = ExtAudioFileGetProperty(audioFileRef,
                                kExtAudioFileProperty_FileDataFormat,
                                &propSize,
                                &inFileASBD);
  if (err != noErr)
    return err;
  
  // setting ExtAudioFile client format
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
  
  // setting buffer
  UInt32 framesToRead = DEFAULT_FRAMES_IN_BUFFER;
  UInt32 sizeOfBuffer = framesToRead * clientASBD.mBytesPerFrame;
  UInt8 *readBuffer = (UInt8 *) malloc(sizeOfBuffer);
  if (readBuffer == NULL) {
    return 1;
  }
  AudioBufferList bufferList;
  bufferList.mNumberBuffers = 1;
  bufferList.mBuffers[0].mData = readBuffer;
  bufferList.mBuffers[0].mDataByteSize = sizeOfBuffer;
  bufferList.mBuffers[0].mNumberChannels = clientASBD.mChannelsPerFrame;
  
  // setting libebur128
  // ExtAudioData userData = {0};
  // userData.mAudioFileRef = &audioFileRef;
  ebur128_state *state = ebur128_init(clientASBD.mChannelsPerFrame,
                                      clientASBD.mSampleRate,
                                      EBUR128_MODE_I | EBUR128_MODE_LRA | EBUR128_MODE_TRUE_PEAK);
  if (state == NULL)
    return 1;
  
  // setting some variables
  UInt32 framesRead = 0;
  UInt64 framesInFile;
  propSize = sizeof(framesInFile);
  err = ExtAudioFileGetProperty(audioFileRef, kExtAudioFileProperty_FileLengthFrames, &propSize, &framesInFile);
  if (err != noErr) {
    return err;
  }
  
  // read audio data
  do {
    err = ExtAudioFileRead(audioFileRef, &framesToRead, &bufferList);
    if (err != noErr) {
      return err;
    }
    int ebu_err = ebur128_add_frames_float(state, (float *)readBuffer, framesToRead);
    if (ebu_err != EBUR128_SUCCESS) {
      return ebu_err;
    }
    framesRead += framesToRead;
    // report progress
    float progress = 100.0 * framesRead / framesInFile;
    CFNumberRef num = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &progress);
    CFNotificationCenterPostNotification(CFNotificationCenterGetLocalCenter(),
                                         PROGRESS_NOTIFICATION,
                                         num,
                                         NULL, true);
    
    CFRelease(num);

  } while (framesRead < framesInFile);
  
  double il;
  err = ebur128_loudness_global(state, &il);
  if (err != 0) {
    return err;
  }
  double lra;
  err = ebur128_loudness_range(state, &lra);
  double maxtp = 0.0;
  for (int i = 0; i < clientASBD.mChannelsPerFrame; i++) {
    double temp;
    err = ebur128_true_peak(state, i, &temp);
    if (err != 0) {
      return err;
    }
    if (temp > maxtp) {
      maxtp = temp;
    }
  }
  *il_p = il;
  *lra_p = lra;
  *maxtp_p = 20 * log10(maxtp);
  // userData.mState = state;
  // userData.mFileOutBuffer = fileOutBuffer;
  // userData.mFileFramesRead = 0;
  // userData.mFramesProduced = 0;
  // userData.mReportIntervalFrames = clientASBD.mSampleRate / 10;
    //cleaning
  //CFRelease(userData.mBlocks);
  free(readBuffer);
  ebur128_destroy(&state);
  ExtAudioFileDispose(audioFileRef);
  if (err == 'insz')
    err = noErr;
  return err;
}
