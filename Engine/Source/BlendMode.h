#pragma once

enum class BlendType 
{
    TRANSPARENCY = 0,
    ADDITIVE,
    MULTIPLICATIVE,
    SUBTRACTIVE,
    PREMULTIPLIED_ALPHA,
    SCREEN
};

void BlendModeFunction(int type);
