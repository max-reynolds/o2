#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>

@interface ViewController : NSOpenGLView
{
    CVDisplayLinkRef displayLink;
}

@end

