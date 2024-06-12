#pragma once
#include "GL/glew.h"
#include <iostream>

void GLClearError();
GLenum glCheckError_(const char* function, const char* file, int line);

#define glCheckError() glCheckError_("", __FILE__, __LINE__)
#define ASSERT(x) if(x) __debugbreak();
#ifdef DEBUG
#define GLCall(x) GLClearError(); x; ASSERT(glCheckError_(#x, __FILE__, __LINE__));
#else
#define GLCall(x) x
#endif
