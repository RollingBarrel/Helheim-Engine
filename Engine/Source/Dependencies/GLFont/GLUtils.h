#pragma once

#include "glew.h"

class GLUtils {
public:
    GLUtils();
    ~GLUtils();

    static void loadShader(const char* shaderSource, GLenum shaderType, GLuint &programId);
};

