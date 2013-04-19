// CliController.h
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

#import <Foundation/Foundation.h>

@interface CliController : NSObject
@property NSString *filePath;
@property double il, lra;
@property Float32 maxTP;
@property SInt32 status;

- (id) initWithPath: (NSString *) thePath;

- (void) doMeasure;

@end
