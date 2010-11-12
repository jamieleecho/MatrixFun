//
//  JCMediaMatrixWindowController.m
//  MediaMatrixExamples
//
//  Created by Jamie Cho on 1/18/08.
//  Copyright 2008 Jamie Cho. All rights reserved.
//

#import <QTKit/QTKit.h>
#import <Quicktime/Quicktime.h>
#import "JCMediaMatrixWindowController.h"

static CIContext *context;

@implementation JCMediaMatrixWindowController

static void init() {
  if (context == nil) {
    context = [CIContext contextWithCGContext:
                    [[NSGraphicsContext currentContext] graphicsPort]
                    options: nil];
    [context retain];
  }
}

- (void)openImage:(id)sender {
  init();
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
  [openPanel setAllowedFileTypes:[NSArray arrayWithObject: @"jpg"]];
  [openPanel beginSheetModalForWindow:[self window] completionHandler: ^(NSInteger result) {
    if (NO == [_imageCompressor openFile:[openPanel filename]]) {
      NSLog(@"oops!!!!");
    } else {
      NSLog(@"ok");
    }}];
}

@end
