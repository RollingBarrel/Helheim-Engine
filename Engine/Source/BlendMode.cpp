#include "BlendMode.h"
#include "glew.h"


void BlendModeFunction(int type)
{
    BlendType blendType = static_cast<BlendType>(type);
    switch (blendType) 
    {
    case BlendType::TRANSPARENCY:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case BlendType::ADDITIVE:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        break;
    case BlendType::MULTIPLICATIVE:
        glBlendFunc(GL_DST_COLOR, GL_ZERO);
        break;
    case BlendType::SUBTRACTIVE:
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
        break;
    case BlendType::PREMULTIPLIED_ALPHA:
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case BlendType::SCREEN:
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
        break;
    default:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    }
}
