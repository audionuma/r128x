// FileReport.m
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

#import "FileReport.h"

@implementation FileReport

- (id) initWithPath: (NSString *) thePath {
    if (self = [super init]) {
        
    
        
        
    self.filePath = thePath;
    self.fileName = [self.filePath lastPathComponent];
        
        
       
        
    
    }
    return self;
    
}

- (void) doMeasure {
    OSStatus err = noErr;
    double il, lra;
    double max_tp;
    //err = CAReader((__bridge CFStringRef)(self.filePath), &il, &lra, &max_tp);
    self.status = ExtAudioReader((__bridge CFStringRef)(self.filePath), &il, &lra, &max_tp);
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
