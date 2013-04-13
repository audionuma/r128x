// CliController.m
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

#import "CliController.h"
#import "ExtAudioProcessor.h"

@implementation CliController
- (id) initWithPath: (NSString *) thePath {
    if (self = [super init]) {
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(progressUpdate:) name:@"R128X_Progress" object:nil];
        self.filePath = thePath;
    }
    return self;
    
}

- (void) doMeasure {
    OSStatus err = noErr;
    double il, lra;
    Float32 max_tp;
    self.status = ExtAudioReader((__bridge CFStringRef)(self.filePath), &il, &lra, &max_tp);
    if (self.status != noErr) {
        self.il = NAN;
        self.lra = NAN;
        self.maxTP = NAN;
    } else {
        self.il = il;
        self.lra = lra;
        self.maxTP = max_tp;
        self.status = err;
        //printf("%.1f %.1f %.1f\n", il, lra, max_tp);
    }
}

- (void) progressUpdate: (NSNotification *) notification {
    NSNumber *f = [notification object];
    //printf("%.1f\n", [f floatValue]);
}
@end
