#pragma once

#include "CNativeRenderQueueListener.h"

// for openGL
#pragma comment(lib , "opengl32.lib")
#pragma comment(lib , "glut32.lib")
#pragma comment(lib , "glew32s.lib")

/*
openGL native render 
 */
class GLNativeRender : public INativeRender
{
public:
    virtual void initRender() {}

    virtual void preRender();
    virtual void postRender();
};
