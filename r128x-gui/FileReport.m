//
//  FileReport.m
//  g_r128x
//
//  Created by manu naudin on 05/01/13.
//  Copyright (c) 2013 manu naudin. All rights reserved.
//

#import "FileReport.h"

@implementation FileReport

- (id) initWithPath: (NSString *) thePath {
    if (self = [super init]) {
        self.filePath = thePath;
    }
    return self;

}

- (void) doMeasure {
    OSStatus err = noErr;
    double il, lra;
    Float32 max_tp;
    //err = CAReader((__bridge CFStringRef)(self.filePath), &il, &lra, &max_tp);
    self.status = CAReader((__bridge CFStringRef)(self.filePath), &il, &lra, &max_tp);
    if (self.status != noErr) {
        //NSLog(self.filePath);
        //printf("File Error %d\n", err);
        self.il = NAN;
        self.lra = NAN;
        self.maxTP = NAN;
    } else {
        self.il = il;
        self.lra = lra;
        self.maxTP = max_tp;
        self.status = err;
        //printf("%f %f %f\n", il, lra, max_tp);
    }
}
@end
