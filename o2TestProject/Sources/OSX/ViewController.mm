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


GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
    
    // Создаем шейдеры
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Загружаем код Вершинного Шейдера из файла
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
    
    // Загружаем код Фрагментного шейдера из файла
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
    
    GLint Result = GL_FALSE;
    int InfoLogLength;
    
    // Компилируем Вершинный шейдер
    printf("Компиляция шейдера: %sn", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
    
    // Выполняем проверку Вершинного шейдера
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        fprintf(stdout, "%sn", &VertexShaderErrorMessage[0]);
    }
    
    // Компилируем Фрагментный шейдер
    printf("Компиляция шейдера: %sn", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
    
    // Проверяем Фрагментный шейдер
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
    }
    
    // Создаем шейдерную программу и привязываем шейдеры к ней
    fprintf(stdout, "Создаем шейдерную программу и привязываем шейдеры к нейn");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    
    // Проверяем шейдерную программу
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
    }
    
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    
    return ProgramID;
}

GLuint programID;
GLuint vertexbuffer;
GLuint VertexArrayID;
GLuint MatrixID;
GLuint textureID;
GLuint TextureIDs;
- (void) initGL
{
    [[self openGLContext] makeCurrentContext];
    
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    /*application = new TestApplication();
    
    applicationBridge = new o2::ApplicationOSXBridge(self);
    application->ConnectOSXViewController(applicationBridge);
    
    application->Launch();*/
    
    
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
//    struct vtx
//    {
//        float x, y, z, r, g, b;
//        
//        vtx() {}
//        vtx(float x, float y, float z, float r, float g, float b):x(x), y(y), z(z), r(r), g(g), b(b) {}
//    };
    
    struct vtx
    {
        float x, y, z;
        unsigned int clr;
        //char r, g, b, a;
        float u, v;
        
        vtx() {}
        vtx(float x, float y, float z, unsigned int clr, float u, float v):x(x), y(y), z(z), clr(clr), u(u), v(v) {}
        //vtx(float x, float y, float z, char r, char g, char b, char a, float u, float v):x(x), y(y), z(z), r(r), g(g), b(b), a(a), u(u), v(v) {}
    };
    
    static const vtx g_vertex_buffer_data2[] = {
//        vtx(0.0f, 0.0f, 0.0f, 0, 0, 0, 150, 0, 0),
//        vtx(100.0f, 0.0f, 0.0f, 0, 0, 0, 150, 1, 0),
//        vtx(0.0f,  100.0f, 0.0f, 0, 0, 0, 150, 0, 1)
        
        vtx(0.0f, 0.0f, 0.0f, 0xffffffff, 0, 0),
        vtx(100.0f, 0.0f, 0.0f, 0x33ffffff, 1, 0),
        vtx(0.0f,  100.0f, 0.0f, 0xffffffff, 0, 1)
    };
    
    o2::Bitmap bitmap;
    bitmap.Load("../../../Assets/ui/UI_Background.png");
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glDisable(GL_CULL_FACE);
    
    // Создадим 1 буфер и поместим в переменную vertexbuffer его идентификатор
    glGenBuffers(1, &vertexbuffer);
    
    // Сделаем только что созданный буфер текущим
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    
    // Передадим информацию о вершинах в OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(vtx)*3, g_vertex_buffer_data2, GL_STREAM_DRAW);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                          0,                  // Атрибут 0. Подробнее об этом будет рассказано в части, посвященной шейдерам.
                          3,                  // Размер
                          GL_FLOAT,           // Тип
                          GL_FALSE,           // Указывает, что значения не нормализованы
                          sizeof(vtx),                  // Шаг
                          (void*)0            // Смещение массива в буфере
                          );
    
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(
                          1,                  // Атрибут 0. Подробнее об этом будет рассказано в части, посвященной шейдерам.
                          4,                  // Размер
                          GL_UNSIGNED_BYTE,           // Тип
                          GL_TRUE,           // Указывает, что значения не нормализованы
                          sizeof(vtx),                  // Шаг
                          (void*)12            // Смещение массива в буфере
                          );
//    glVertexAttribPointer(
//                          1,                  // Атрибут 0. Подробнее об этом будет рассказано в части, посвященной шейдерам.
//                          4,                  // Размер
//                          GL_UNSIGNED_BYTE,           // Тип
//                          GL_TRUE,           // Указывает, что значения не нормализованы
//                          sizeof(vtx),                  // Шаг
//                          (void*)12            // Смещение массива в буфере
//                          );
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                          2,                  // Атрибут 0. Подробнее об этом будет рассказано в части, посвященной шейдерам.
                          2,                  // Размер
                          GL_FLOAT,           // Тип
                          GL_FALSE,           // Указывает, что значения не нормализованы
                          sizeof(vtx),                  // Шаг
                          (void*)16            // Смещение массива в буфере
                          );

    programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
    glUseProgram(programID);
    MatrixID = glGetUniformLocation(programID, "MVP");
    
    glGenTextures(1, &textureID);
    
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.GetSize().x, bitmap.GetSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.GetData());
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    TextureIDs  = glGetUniformLocation(programID, "myTextureSampler");
}

- (void)reshape
{
    [super reshape];
    
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    //applicationBridge->OnViewDidLayout();
    
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
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

void mtxMultiply(float* ret, const float* lhs, const float* rhs)
{
    // [ 0 4  8 12 ]   [ 0 4  8 12 ]
    // [ 1 5  9 13 ] x [ 1 5  9 13 ]
    // [ 2 6 10 14 ]   [ 2 6 10 14 ]
    // [ 3 7 11 15 ]   [ 3 7 11 15 ]
    ret[ 0] = lhs[ 0]*rhs[ 0] + lhs[ 4]*rhs[ 1] + lhs[ 8]*rhs[ 2] + lhs[12]*rhs[ 3];
    ret[ 1] = lhs[ 1]*rhs[ 0] + lhs[ 5]*rhs[ 1] + lhs[ 9]*rhs[ 2] + lhs[13]*rhs[ 3];
    ret[ 2] = lhs[ 2]*rhs[ 0] + lhs[ 6]*rhs[ 1] + lhs[10]*rhs[ 2] + lhs[14]*rhs[ 3];
    ret[ 3] = lhs[ 3]*rhs[ 0] + lhs[ 7]*rhs[ 1] + lhs[11]*rhs[ 2] + lhs[15]*rhs[ 3];
    
    ret[ 4] = lhs[ 0]*rhs[ 4] + lhs[ 4]*rhs[ 5] + lhs[ 8]*rhs[ 6] + lhs[12]*rhs[ 7];
    ret[ 5] = lhs[ 1]*rhs[ 4] + lhs[ 5]*rhs[ 5] + lhs[ 9]*rhs[ 6] + lhs[13]*rhs[ 7];
    ret[ 6] = lhs[ 2]*rhs[ 4] + lhs[ 6]*rhs[ 5] + lhs[10]*rhs[ 6] + lhs[14]*rhs[ 7];
    ret[ 7] = lhs[ 3]*rhs[ 4] + lhs[ 7]*rhs[ 5] + lhs[11]*rhs[ 6] + lhs[15]*rhs[ 7];
    
    ret[ 8] = lhs[ 0]*rhs[ 8] + lhs[ 4]*rhs[ 9] + lhs[ 8]*rhs[10] + lhs[12]*rhs[11];
    ret[ 9] = lhs[ 1]*rhs[ 8] + lhs[ 5]*rhs[ 9] + lhs[ 9]*rhs[10] + lhs[13]*rhs[11];
    ret[10] = lhs[ 2]*rhs[ 8] + lhs[ 6]*rhs[ 9] + lhs[10]*rhs[10] + lhs[14]*rhs[11];
    ret[11] = lhs[ 3]*rhs[ 8] + lhs[ 7]*rhs[ 9] + lhs[11]*rhs[10] + lhs[15]*rhs[11];
    
    ret[12] = lhs[ 0]*rhs[12] + lhs[ 4]*rhs[13] + lhs[ 8]*rhs[14] + lhs[12]*rhs[15];
    ret[13] = lhs[ 1]*rhs[12] + lhs[ 5]*rhs[13] + lhs[ 9]*rhs[14] + lhs[13]*rhs[15];
    ret[14] = lhs[ 2]*rhs[12] + lhs[ 6]*rhs[13] + lhs[10]*rhs[14] + lhs[14]*rhs[15];
    ret[15] = lhs[ 3]*rhs[12] + lhs[ 7]*rhs[13] + lhs[11]*rhs[14] + lhs[15]*rhs[15];
}

- (void) drawView
{
    [[self openGLContext] makeCurrentContext];
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    //application->ProcessFrame();
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    //glUseProgram(programID);
    
    // 1rst attribute buffer : vertices
//    glEnableVertexAttribArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//    glVertexAttribPointer(
//                          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//                          3,                  // size
//                          GL_FLOAT,           // type
//                          GL_FALSE,           // normalized?
//                          0,                  // stride
//                          (void*)0            // array buffer offset
//                          );
//
//    
//    // Вывести треугольник!
//    glDrawArrays(GL_TRIANGLES, 0, 3); // Начиная с вершины 0, всего 3 вершины -> один треугольник
//    
//    glDisableVertexAttribArray(0);
    
    //glBindVertexArray(VertexArrayID);
    
    static float f = 0;
    f += 0.01f;
    
    GLfloat* g_vertex_buffer_data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    
    if (g_vertex_buffer_data)
    {
        g_vertex_buffer_data[0] = 0;
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    else
    {
        auto err = glGetError();
        printf("error %x\n", err);
    }
    
    float mvp[16] = {
        0.5f, 0, 0, 0,
        0, 0.5f, 0, 0,
        0, 0, 0.5f, 0,
        0, 0, 0, 1
    };
    
    
    NSRect viewRectPoints = [self bounds];
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
    o2::Vec2F resf(viewRectPixels.size.width, viewRectPixels.size.height);
    
    float projMat[16];
    o2::Math::OrthoProjMatrix(projMat, 0.0f, (float)resf.x, (float)resf.y, 0.0f, 0.0f, 10.0f);
    glViewport(0, 0, resf.x, resf.y);
    
    float modelMatrix[16] =
    {
        1,           0,            0, 0,
        0,          -1,            0, 0,
        0,           0,            1, 0,
        o2::Math::Round(resf.x*0.5f), o2::Math::Round(resf.y*0.5f), -1, 1
    };
    
    o2::Basis defaultCameraBasis((o2::Vec2F)resf*-0.5f, o2::Vec2F::Right()*resf.x, o2::Vec2F().Up()*resf.y);
    o2::Basis camTransf; // = mCamera.GetBasis().Inverted()*defaultCameraBasis;
    
    float camTransfMatr[16] =
    {
        camTransf.xv.x,   camTransf.xv.y,   0, 0,
        camTransf.yv.x,   camTransf.yv.y,   0, 0,
        0,                0,                0, 0,
        camTransf.offs.x, camTransf.offs.y, 0, 1
    };
    
    float pp[16];
    mtxMultiply(pp, modelMatrix, camTransfMatr);
    mtxMultiply(mvp, projMat, pp);
    
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, mvp);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(TextureIDs, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    CGLFlushDrawable([[self openGLContext] CGLContextObj]);
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
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
