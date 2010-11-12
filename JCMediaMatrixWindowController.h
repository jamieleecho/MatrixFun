//
//  JCMediaMatrixWindowController.h
//  MediaMatrixExamples
//
//  Created by Jamie Cho on 1/18/08.
//  Copyright 2008 Jamie Cho. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "JCImageCompressor.h"


@interface JCMediaMatrixWindowController : NSWindowController {
  IBOutlet JCImageCompressor *_imageCompressor;
}

- (void)openImage:(id)sender;
@end
