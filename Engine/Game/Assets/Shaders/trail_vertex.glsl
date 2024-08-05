#version 460 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in float distance;
layout (location = 3) in vec3 direction;

out vec2 TexCoords;
out vec4 VertexColor;

uniform mat4 viewProj;
uniform float minDist;
uniform float maxDist;
uniform float scrollUV;
uniform int isUVScrolling;
uniform int colorSize;
uniform int widthSize;
uniform int isCurve;
uniform int isRandWidth;
uniform float minWidth;
uniform float maxWidth;

struct ColorPoint
{
    float pos; 
    float padding[3];
    vec4 col;
};

readonly layout(std430, binding = 30) buffer ColorData 
{
    ColorPoint pointColor[];
};

struct WidthPoint
{
    float pos;
    float width;
};

readonly layout(std430, binding = 31) buffer WidthData 
{
    WidthPoint pointWidth[];
};

void calculateTexCoords(float deltaPos);
vec4 calculateVertexColor(float deltaPos);
vec3 modifyWidth(float deltaPos);
float CurveValue(float deltaPos);
float GetDeltaPosition();
float rand();

void main()
{
    float deltaPos = GetDeltaPosition();
    calculateTexCoords(deltaPos);
    VertexColor = calculateVertexColor(deltaPos);
    vec3 newVertex = modifyWidth(deltaPos);
    gl_Position = viewProj * vec4(newVertex, 1.0);
}

void calculateTexCoords(float deltaPos) {
    TexCoords = vec2(
        isUVScrolling > 0 ? texCoord.x * scrollUV : deltaPos,
        texCoord.y
    );
}

float GetDeltaPosition()
{
    return clamp((distance - minDist) / (maxDist - minDist), 0.0, 1.0);
}

vec4 calculateVertexColor(float deltaPos) 
{
    if (colorSize == 0) return vec4(1.0);
    int lowerIndex = 0, upperIndex = colorSize - 1;
    for (int i = 0; i < colorSize; ++i) {
        if (pointColor[i].pos <= deltaPos) {
            lowerIndex = i;
        } else {
            upperIndex = i;
            break;
        }
    }
    if (lowerIndex == upperIndex) {
        return pointColor[lowerIndex].col;
    }

    float delta = (deltaPos - pointColor[lowerIndex].pos) / (pointColor[upperIndex].pos - pointColor[lowerIndex].pos);
    return mix(pointColor[lowerIndex].col, pointColor[upperIndex].col, delta);
}

vec3 modifyWidth(float deltaPos)
{
    float percentage = CurveValue(deltaPos);
    float scalar = -(1 - percentage) * minWidth * 0.5;
    if (isRandWidth > 0) 
    {
        float randWidth = rand() * (maxWidth - minWidth);
        scalar += percentage * randWidth * 0.5;
    }
    return vertex + direction * (scalar * (texCoord.y * 2 - 1));
}

float CurveValue(float deltaPos) 
{
    if (isCurve < 1) return 1.0;
    if (widthSize < 2) return 0.0;
    if (deltaPos <= pointWidth[0].pos) return pointWidth[0].width;

    for (int i = 0; i < widthSize - 1; ++i) {
        if (deltaPos < pointWidth[i + 1].pos) 
        {
            float d = (deltaPos - pointWidth[i].pos) / (pointWidth[i + 1].pos - pointWidth[i].pos);
            return mix(pointWidth[i].width, pointWidth[i + 1].width, d);
        }
    }

    return pointWidth[widthSize - 1].width;
}

float rand()
{
    return 1.0;
}
