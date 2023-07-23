// MainWindow.m
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

#import "MainWindow.h"

@implementation MainWindow
- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender {
  NSPasteboard *pboard;
  NSDragOperation sourceDragMask;
  sourceDragMask = [sender draggingSourceOperationMask];
  pboard = [sender draggingPasteboard];
  if ([[pboard types] containsObject:NSPasteboardTypeFileURL]) {
    NSArray *files = [pboard propertyListForType:NSPasteboardTypeFileURL];

    // Depending on the dragging source and modifier keys,

    // the file data may be copied or linked

    if (sourceDragMask & NSDragOperationLink) {
      // NSLog(@"dragA %ld", [files count]);
      [[NSNotificationCenter defaultCenter]
          postNotificationName:@"R128X_FilesDropped"
                        object:files];
    }

    [progressIndicator setHidden:NO];
  }

  return YES;
}
- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
  NSPasteboard *pboard;

  NSDragOperation sourceDragMask;

  sourceDragMask = [sender draggingSourceOperationMask];

  pboard = [sender draggingPasteboard];

  if ([[pboard types] containsObject:NSPasteboardTypeFileURL]) {
    if (sourceDragMask & NSDragOperationLink) {
      return NSDragOperationLink;

    } else if (sourceDragMask & NSDragOperationCopy) {
      return NSDragOperationCopy;
    }
  }

  return NSDragOperationNone;
}

@end
