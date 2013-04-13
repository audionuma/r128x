//
//  MainWindow.m
//  g_r128x
//
//  Created by manu naudin on 06/01/13.
//  Copyright (c) 2013 manu naudin. All rights reserved.
//

#import "MainWindow.h"

@implementation MainWindow
- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    NSPasteboard *pboard;
    NSDragOperation sourceDragMask;
    sourceDragMask = [sender draggingSourceOperationMask];
    pboard = [sender draggingPasteboard];
    if ( [[pboard types] containsObject:NSFilenamesPboardType] ) {
        NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
        
        
        
        // Depending on the dragging source and modifier keys,
        
        // the file data may be copied or linked
        
        if (sourceDragMask & NSDragOperationLink) {
            //NSLog(@"dragA %ld", [files count]);
            [[NSNotificationCenter defaultCenter] postNotificationName:@"R128X_FilesDropped"object:files];
            
        }
        
    }
    
    return YES;
    
}
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender {
    
    NSPasteboard *pboard;
    
    NSDragOperation sourceDragMask;
    
    
    
    sourceDragMask = [sender draggingSourceOperationMask];
    
    pboard = [sender draggingPasteboard];
    
    
    
    
    
    if ( [[pboard types] containsObject:NSFilenamesPboardType] ) {
        
        if (sourceDragMask & NSDragOperationLink) {
            
            return NSDragOperationLink;
            
        } else if (sourceDragMask & NSDragOperationCopy) {
            
            return NSDragOperationCopy;
            
        }
        
    }
    
    return NSDragOperationNone;
    
}

@end
