// main.m
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


#import <Foundation/Foundation.h>
#import "CliController.h"

int main(int argc, const char * argv[])
{
    
    @autoreleasepool {
        if (argc < 2) {
            printf("Missing arguments\nYou should specify at least one audio file\nr128x /some/file\n");
            return 0;
        } else {
            CliController *controller = [[CliController alloc] init];
            [[NSNotificationCenter defaultCenter] addObserver:controller selector:@selector(progressUpdate:) name:@"R128X_Progress" object:nil];
            
            for (int i = 1; i < argc; i++) {
                [controller setFilePath:[NSString stringWithUTF8String:argv[i]]];
                [controller doMeasure];
                NSString *fileName = [[controller filePath] lastPathComponent];
                printf("%s\t %.1f\t%.1f\t%.1f\n", [fileName UTF8String], [controller il], [controller lra], [controller maxTP]);
            }
        }
        
    }
    return 0;
}

