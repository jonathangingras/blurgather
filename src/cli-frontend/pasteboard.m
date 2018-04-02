#include <unistd.h>
#include <stdio.h>
#include <string.h>

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "utils.h"

@interface PasteboardWriter : NSObject
{
  NSPasteboard *board;
}

- (void) write:(const char*) data;
- (void) clear;
@end

@implementation PasteboardWriter

- (id) init {
  self = [super init];
  board = [NSPasteboard generalPasteboard];
  [board declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:nil];
  return self;
}

- (oneway void) release {
  [board release];
  [super release];
}

- (void) write:(const char*) data {
  [self clear];
  [board setString:[[NSString alloc] initWithCString:data encoding:NSUTF8StringEncoding] forType:NSStringPboardType];
}

- (void) clear {
  [board clearContents];
}

@end


void send_to_clipboard(const char* password) {
  @autoreleasepool {
    id writer = [[PasteboardWriter alloc] init];
    [writer write: password];
  }
}

void clear_clipboard() {
  @autoreleasepool {
    id writer = [[PasteboardWriter alloc] init];
    [writer clear];
  }
}

