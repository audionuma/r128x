//
//  FileReport.h
//  g_r128x
//
//  Created by manu naudin on 05/01/13.
//  Copyright (c) 2013 manu naudin. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "fileprocessor.h"

@interface FileReport : NSObject
@property NSString *filePath;
@property double il, lra;
@property Float32 maxTP;
@property SInt32 status;

- (id) initWithPath: (NSString *) thePath;

- (void) doMeasure;
@end
