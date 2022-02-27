//
//  JCImageCompressor.mm
//  MediaMatrixExamples
//
//  Created by Jamie Cho on 2010-11-05.
//  Copyright 2010 Jamie Cho. All rights reserved.
//

#import <CoreImage/CoreImage.h>
#import "JCImageCompressor.h"
#include "Matrix.h"

#include <vector>

@implementation JCImageCompressor

@synthesize inputImage;
@synthesize outputImage;

+(JCImageCompressorBasisFunction) powerFunctionWithNumXTerms:(NSInteger)numXTerms numYTerms:(NSInteger)numYTerms {
  // Arbitrarily set the maximum power to 10
  __block double cx = (numXTerms < 2) ? 0 : 10 / (numXTerms - 1);
  __block double cy = (numYTerms < 2) ? 0 : 10 / (numYTerms - 1);
  if (cx > 2) cx = 2;
  if (cy > 2) cy = 2;

  return [[(^(double x, double y, int tx, int ty) {
    double x0 = ((cx * tx) == 0) ? 1 : pow(x, cx * tx);
    double y0 = ((cy * ty) == 0) ? 1 : pow(y, cy * ty);    
    return x0 * y0;
  }) copy] autorelease];
}

-(id) init {
  if (self = [super init]) {
    numXTerms = 3;
    numYTerms = 3;
  }
  return self;
}

-(void) applyFilter {
  if (self.inputImage == nil) {
    self.outputImage = nil;
    return;
  }
  
  // Get a grey scale component version of the image
  NSBitmapImageRep *imageRep = [[NSBitmapImageRep alloc] initWithCIImage:[[[self.inputImage representations] objectAtIndex:0] CIImage]];
  CGImageRef cgImage = [imageRep CGImage];
  CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceGray();
  CGContextRef cgContext = CGBitmapContextCreate (NULL, self.inputImage.size.width, self.inputImage.size.height, 8, self.inputImage.size.width, colorSpaceRef, 0);
  CGContextDrawImage(cgContext, CGRectMake(0, 0, self.inputImage.size.width, self.inputImage.size.height), cgImage);
  void *data = CGBitmapContextGetData(cgContext);
  [imageRep release];

  JCImageCompressorBasisFunction basisFunction = [JCImageCompressor powerFunctionWithNumXTerms:numXTerms numYTerms:numYTerms];
  JCImageCompressorSampleFunction sampler = [self sampleFunctionFromBasisFunction:basisFunction];
  NSSize size = self.inputImage.size;
  int width = (int)size.width;
  int height = (int)size.height;
  uint8 *data8 = (uint8 *)data;
  int offset = 0;
  for(int yy=0; yy<height; yy++) {
    for(int xx=0; xx<width; xx++, offset++) {
      double value = sampler(xx, yy);
      data8[offset] = (value < 0) ? 0 : ((value > 255) ? 255 : value);
    }
  }

  // Set as output NSImage
  NSBitmapImageRep *bitmap = [[[NSBitmapImageRep alloc] initWithBitmapDataPlanes:NULL pixelsWide:self.inputImage.size.width pixelsHigh:self.inputImage.size.height bitsPerSample:8 samplesPerPixel:1 hasAlpha:NO isPlanar:NO colorSpaceName:NSCalibratedWhiteColorSpace bytesPerRow:self.inputImage.size.width bitsPerPixel:8] autorelease];
  memcpy([bitmap bitmapData], data, self.inputImage.size.width * self.inputImage.size.height);
  NSImage *newOutputImage = [[[NSImage alloc] initWithSize:NSMakeSize(self.inputImage.size.width, self.inputImage.size.height)] autorelease];
  [newOutputImage addRepresentation:bitmap];  
  self.outputImage = newOutputImage;
  
  CFRelease(cgContext);
  CFRelease(colorSpaceRef);
}

-(NSInteger)numXTerms { return numXTerms; }
-(void) setNumXTerms:(NSInteger) num {
  numXTerms = num;
  [self applyFilter];
}

-(NSInteger)numYTerms { return numYTerms; }
-(void) setNumYTerms:(NSInteger) num {
  numYTerms = num;
  [self applyFilter];
}

-(BOOL) openFile:(NSString *)path {
  NSFileHandle *fileHandle = [NSFileHandle fileHandleForReadingAtPath:path];
  if (fileHandle == nil) return NO;
  NSData *data = [fileHandle readDataToEndOfFile];
  if (data == nil) return NO;
  CIImage *ciImage = [CIImage imageWithData:data];
  if (ciImage == nil) return NO;
  
  // Simple CoreImage example to convert an image to greyscale
  CIFilter *filter = [CIFilter filterWithName:@"CIColorControls"];
  [filter setValue:ciImage forKey:kCIInputImageKey];
  [filter setValue:[NSNumber numberWithDouble:0.0] forKey:@"inputSaturation"];
  [filter setValue:[NSNumber numberWithDouble:0.0] forKey:@"inputBrightness"];
  [filter setValue:[NSNumber numberWithDouble:1.0] forKey:@"inputContrast"];
  CIImage *ciOutputImage = [filter valueForKey:kCIOutputImageKey];
  CGSize size = [ciOutputImage extent].size;
  NSImage *nsImage = [[[NSImage alloc] initWithSize:NSMakeSize(size.width, size.height)] autorelease];
  NSCIImageRep *rep = [NSCIImageRep imageRepWithCIImage:ciOutputImage];
  [nsImage addRepresentation:rep];
  self.inputImage = nsImage;
  
  [self applyFilter];
  
  return YES;
}

-(JCImageCompressorSampleFunction) sampleFunctionFromBasisFunction:(JCImageCompressorBasisFunction)basisFunction {
  __block JCImageCompressorBasisFunction f = [basisFunction retain];
  __block NSInteger numX = numXTerms;
  __block NSInteger numY = numYTerms;
  NSInteger numCoefficients = numX * numY;
  std::vector<double> coefficients;
  NSSize size = self.inputImage.size;
  int width = (int)size.width;
  int height = (int)size.height;

  // Simple CoreGraphics example to convert an image to 8-bit grey-scale
  NSBitmapImageRep *imageRep = [[NSBitmapImageRep alloc] initWithCIImage:[[[self.inputImage representations] objectAtIndex:0] CIImage]];
  CGImageRef cgImage = [imageRep CGImage];
  CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceGray();
  CGContextRef cgContext = CGBitmapContextCreate (NULL, self.inputImage.size.width, self.inputImage.size.height, 8, self.inputImage.size.width, colorSpaceRef, 0);
  CGContextDrawImage(cgContext, CGRectMake(0, 0, self.inputImage.size.width, self.inputImage.size.height), cgImage);
  uint8 *data = (uint8 *)CGBitmapContextGetData(cgContext);
  [imageRep release];

  // Setup the sample matrix and result vector. To do this, for every pixel...
  jcho::Matrix<double> m(width * height, (int)(numY * numX));
  jcho::Matrix<double> v(width * height, 1);
  int row = 0;
  for(int yy=0; yy<height; yy++) {
    for(int xx=0; xx<width; xx++, row++) {
      // For every term...
      int column = 0;
      for(int ii=0; ii<numY; ii++) {
        for(int jj=0; jj<numX; jj++, column++) {
          // Set an entry in the matrix corresponding to that term
          double value =  f(xx, yy, jj, ii);
          m.set(row, column, value);
        }
      }
      
      // Set the pixel value
      v.set(row, 0, data[row]);
    }
  }

  jcho::Matrix<double> mm(3, 2);
  jcho::Matrix<double> vv(3, 1);
  mm.set(0, 0, 1);
  mm.set(0, 1, 0);
  mm.set(1, 0, 1);
  mm.set(1, 1, 1);
  mm.set(2, 0, 1);
  mm.set(2, 1, 2);
  vv.set(0, 0, 0);
  vv.set(1, 0, 1);
  vv.set(2, 0, 2);
  jcho::Matrix<double> aa = vv.linear_least_squares(mm);
  for(int ii=0; ii<aa.m(); ii++)
    NSLog(@"%lf", aa.get(ii, 0));  
  
  // Compute the coefficients
  jcho::Matrix<double> a = v.linear_least_squares(m);
  coefficients.resize(numCoefficients, 0);
  NSLog(@"%d %d", a.m(), a.n());
  for(int ii=0; ii<numCoefficients; ii++) {
    coefficients[ii] = a.get(ii, 0);
    NSLog(@"%lf", coefficients[ii]);
  }

  // Clean up
  CFRelease(cgContext);
  CFRelease(colorSpaceRef);

  return [[(^(int x, int y) { 
    double sum = 0;
    int row = 0;
    for(int ii=0; ii<numY; ii++) {
      for(int jj=0; jj<numX; jj++, row++) {
        sum += coefficients[row] * f(x, y, jj, ii);
      }
    }
    return sum;
  }) copy] autorelease];
}

@end
