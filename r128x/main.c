//
//  main.c
//  r128x
//
//  Created by manu naudin on 16/09/12.
//  Copyright (c) 2012 Manuel Naudin. All rights reserved.
//

#include "fileprocessor.h"

int main(int argc, const char * argv[])
{
    if (argc < 2)
    {
        printf("Missing arguments\nYou should specify at least one audio file\nr128x /some/file\n");
        return 0;
    }
    
    OSStatus err = noErr;
    
    for(int i = 1; i < argc; i++)
    {
        CFStringRef filePath = CFStringCreateWithCString(kCFAllocatorDefault, argv[i], kCFStringEncodingUTF8);
        err = CAReader(filePath);
        if (err != noErr)
            printf("An error occured while trying to open/read/process file %s\n", argv[i]);
    }
    return 0;
}

