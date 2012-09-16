# r128x, a command line tool for loudness measurement of files on Mac OSX Intel. #
v 0.1 2012-09-16

## Description ##
r128x is a command line tool for measuring Integrated Loudness, Loudness Range and Maximum True Peak of audio files on the Mac OS X system with an Intel processor.
It uses libebeur128 (https://github.com/jiixyj/libebur128) for the Integrated Loudness and Loudness Range measures.
It uses CoreAudio's AudioConverter service to oversample audio data and obtain the Maximum True Peak.
It uses CoreAudio ExtAudioFile's service to read the samples from an audio file.

## Installation ##
You can build the executable from the Xcode project, or use the provided executable.
Just drag the executable file to your hard drive.
/usr/local/bin/ should be a nice place to put the binary if you want it to be in your $PATH.

## Usage ##
r128x /some/audio/file.wav
Will print out the full file path, the Integrated Loudness in LUFS, the Loudness Range in LU, the Maximum True Peak in dBTP.
Will print an error message in case of unsupported file or if an error occurs during processing.

## Issues ##
Channel mapping is static, using the default libebur128 channel mapping :
L / R / C / LFE / Ls / Rs.
You will have incorrect results if your file's channels mapping is different.
As r128x uses CoreAudio to read audio files, only file formats/codecs supported by CoreAudio are supported by r128x.