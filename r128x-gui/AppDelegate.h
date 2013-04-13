// AppDelegate.h
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

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;

@property (unsafe_unretained) IBOutlet NSButton *fileSelectBtn;
@property (unsafe_unretained) IBOutlet NSTextField *filesInQueueFld;
@property (unsafe_unretained) IBOutlet NSTextField *currentFileProcessedFld;
@property (unsafe_unretained) IBOutlet NSArrayController *myArrayController;
@property (unsafe_unretained) IBOutlet NSTableView *myTableView;
@property (unsafe_unretained) IBOutlet NSLevelIndicatorCell *progressIndicator;
@property NSMutableArray *filesQueue;
@property NSThread *workingThread;

- (IBAction)selectBtnPressed:(id)sender;

- (void) timerFire;

- (void) updateView;

- (void) droppedFiles: (NSNotification *) notification;

- (void) processedFiles: (NSNotification *) notification;

- (void) progressUpdate: (NSNotification *) notification;

@end
