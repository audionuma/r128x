// AppDelegate.m
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

#import "AppDelegate.h"
#import "FileReport.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    //self.maintainer = [[FilesMaintainer alloc] init];
    self.filesQueue =[NSMutableArray arrayWithCapacity:1];
    [self.filesInQueueFld setStringValue:@"0"];
    [self.currentFileProcessedFld setStringValue:@""];
    [self.progressIndicator setIntegerValue:0];
    [NSTimer scheduledTimerWithTimeInterval:1.0
                                     target:self
                                   selector:@selector(timerFire)
                                   userInfo:nil
                                    repeats:true];
    //self.workingThread = [[NSThread alloc] initWithTarget:self.maintainer
    //selector:@selector(processNextFileInQueue:)
    //  object:self];
    self.workingThread = nil;
    //[self.receiver registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(droppedFiles:) name:@"R128X_FilesDropped" object:nil];
    [self.window registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(droppedFiles:) name:@"R128X_FilesDropped" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(processedFiles:) name:@"R128X_FileProcessed" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(progressUpdate:) name:@"R128X_Progress" object:nil];

}
- (IBAction)selectBtnPressed:(id)sender {
    //NSLog(@"pressed");
    NSOpenPanel *fileChooser = [NSOpenPanel openPanel];
    [fileChooser setCanChooseDirectories:false];
    [fileChooser setCanChooseFiles:true];
    [fileChooser setAllowsMultipleSelection:true];
    if ([fileChooser runModal] == NSOKButton) {
        for (NSURL *fileURL in [fileChooser URLs]) {
            [self.filesQueue addObject:[fileURL path]];
        }
    }
    //[self.maintainer addToQueue:@"string"];
    [self updateView];
}

- (void) timerFire {
    //NSLog(@"fire !************************");
    //NSLog([self.workingThread isExecuting] ? @"Yes" : @"No");
    if ([self.filesQueue count] > 0 && ![self.workingThread isExecuting]) {
        //[self setIsWorking:true];
        //[self.maintainer processNextFileInQueue:self];
        self.workingThread = [[NSThread alloc] initWithTarget:self
                                                     selector:@selector(processNextFileInQueue:)
                                                       object:self];
        [self.workingThread start];
    } else {
        //NSLog(@"nothing to do");
    }
    [self updateView];
}

- (void) updateView {
    [self.progressIndicator setIntegerValue:0];
    [self.filesInQueueFld setStringValue:[NSString stringWithFormat:@"%ld", [self.filesQueue count]]];
}

- (void) droppedFiles: (NSNotification *) notification {
    NSArray *files = [notification object];
    //NSLog(@"dropped %ld", [files count]);
    for (NSString *path in files) {
        BOOL exist, isdir;
        exist = [[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isdir];
        if (exist && !isdir) {
            [self.filesQueue addObject:path];
            [self updateView];
        }
    }
}
- (void) processedFiles: (NSNotification *) notification {
    [self updateView];
    FileReport *f = [notification object];
    [self.myArrayController addObject:f];
}
- (void) processNextFileInQueue:(id)launcher {
    NSString *file = [self.filesQueue objectAtIndex:0];
    [self.filesQueue removeObjectAtIndex:0];
    FileReport *report = [[FileReport alloc] initWithPath:file];
    [self.currentFileProcessedFld setStringValue:file];
    [report doMeasure];
    //[self.filesProcessed addObject:report];
    [self.currentFileProcessedFld setStringValue:@""];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"R128X_FileProcessed"object:report];
    //[launcher setIsWorking:false];
}
- (void) progressUpdate: (NSNotification *) notification {
    //NSLog(@"progress");
    NSNumber *f = [notification object];
    [self.progressIndicator setIntegerValue:[f integerValue]];
    //NSLog(@"%f", [f floatValue]);
}


@end
