#import "ViewController.h"

#import "TestApplication.h"
#import "Application/OSX/ApplicationBridge.h"
#include "Utils/Bitmap.h"
#import <OpenGL/gl3.h>

@interface ViewController ()
{
    TestApplication* application;
    o2::ApplicationOSXBridge* applicationBridge;
}
@end

@implementation ViewController

- (CVReturn) getFrameForTime:(const CVTimeStamp*)outputTime
{
    @autoreleasepool {
        [self drawView];
    }
    return kCVReturnSuccess;
}

static CVReturn DisplayLinkCallback(CVDisplayLinkRef displayLink,
                                      const CVTimeStamp* now,
                                      const CVTimeStamp* outputTime,
                                      CVOptionFlags flagsIn,
                                      CVOptionFlags* flagsOut,
                                      void* displayLinkContext)
{
    CVReturn result = [(__bridge ViewController*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

- (void) awakeFromNib
{
    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,
        0
    };
    
    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    
    if (!pf)
        NSLog(@"No OpenGL pixel format");
	   
    NSOpenGLContext* context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
    
    [self setPixelFormat:pf];
    [self setOpenGLContext:context];
    [self setWantsBestResolutionOpenGLSurface:YES];
}

- (void) prepareOpenGL
{
    [super prepareOpenGL];
    
    [self initGL];
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &DisplayLinkCallback, (__bridge void*)self);
    
    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    
    CVDisplayLinkStart(displayLink);
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(windowWillClose:)
                                                 name:NSWindowWillCloseNotification
                                               object:[self window]];
}

- (void) windowWillClose:(NSNotification*)notification
{
    CVDisplayLinkStop(displayLink);
}

- (void) initGL
{
    [[self openGLContext] makeCurrentContext];
    
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    application = new TestApplication();
    
    applicationBridge = new o2::ApplicationOSXBridge(self);
    application->ConnectOSXViewController(applicationBridge);
    
    application->Launch();
    
    applicationBridge->OnViewDidLayout();
    
    [[self window] setAcceptsMouseMovedEvents:YES];
}

- (void)reshape
{
    [super reshape];
    
    //CGLLockContext([[self openGLContext] CGLContextObj]);
    
    applicationBridge->OnViewDidLayout();
    
    //CGLUnlockContext([[self openGLContext] CGLContextObj]);
}


- (void)renewGState
{
    [[self window] disableScreenUpdatesUntilFlush];
    [super renewGState];
}

- (void) drawRect: (NSRect) theRect
{
    [self drawView];
}

- (void) drawView
{
    [[self openGLContext] makeCurrentContext];
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    applicationBridge->ApplyInputMessages();
    application->ProcessFrame();
    
    CGLFlushDrawable([[self openGLContext] CGLContextObj]);
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
}

- (void)mouseDown:(NSEvent *)event
{
    NSPoint pt = [self convertPoint:[event locationInWindow] fromView:nil];
    NSRect viewRectPoints = [self bounds];
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    applicationBridge->CursorPressed(o2::Vec2F(o2::Math::Floor(pt.x - viewRectPixels.size.width/2),
                                               o2::Math::Floor(pt.y - viewRectPixels.size.height/2)));
}

- (void)mouseDragged:(NSEvent *)event {
    
    NSPoint pt = [self convertPoint:[event locationInWindow] fromView:nil];
    NSRect viewRectPoints = [self bounds];
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    applicationBridge->SetCursorPos(o2::Vec2F(o2::Math::Floor(pt.x - viewRectPixels.size.width/2),
                                              o2::Math::Floor(pt.y - viewRectPixels.size.height/2)));
}

- (void)mouseMoved:(NSEvent *)event
{
    NSPoint pt = [self convertPoint:[event locationInWindow] fromView:nil];
    NSRect viewRectPoints = [self bounds];
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    applicationBridge->SetCursorPos(o2::Vec2F(o2::Math::Floor(pt.x - viewRectPixels.size.width/2),
                                              o2::Math::Floor(pt.y - viewRectPixels.size.height/2)));
}

- (void)mouseUp:(NSEvent *)event
{
    applicationBridge->CursorReleased();
}

- (BOOL)acceptsFirstResponder {
    
    return YES;
}

- (void) dealloc
{
    applicationBridge->OnViewUnload();
    delete application;
    delete applicationBridge;
    
    CVDisplayLinkStop(displayLink);    
    CVDisplayLinkRelease(displayLink);
}

@end
