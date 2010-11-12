//
//  JCImageCompressor.h
//  MediaMatrixExamples
//
//  Created by Jamie Cho on 2010-11-05.
//  Copyright 2010 CS2 Technologies LLC. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef double (^JCImageCompressorBasisFunction)(double, double, int, int);
typedef double (^JCImageCompressorSampleFunction)(int, int);

@interface JCImageCompressor : NSObject {
  @private NSImage *inputImage;
  @private NSImage *outputImage;
  @private NSInteger numXTerms;
  @private NSInteger numYTerms;
}

+(JCImageCompressorBasisFunction) powerFunctionWithNumXTerms:(int)numXTerms numYTerms:(int)numYTerms;

@property (readwrite,retain) NSImage *inputImage;
@property (readwrite,retain) NSImage *outputImage;
@property (readwrite,assign) NSInteger numXTerms;
@property (readwrite,assign) NSInteger numYTerms;

-(BOOL) openFile:(NSString *)path;

-(JCImageCompressorSampleFunction) sampleFunctionFromBasisFunction:(JCImageCompressorBasisFunction)basisFunction;

@end
