// [src] https://github.com/ocornut/imgui/issues/123
// [src] https://github.com/ocornut/imgui/issues/55

// v1.22 - flip button; cosmetic fixes
// v1.21 - oops :)
// v1.20 - add iq's interpolation code
// v1.10 - easing and colors        
// v1.00 - jari komppa's original

#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"

#include "imgui_internal.h"
#include <cmath>
#include "float2.h"

/* To use, add this prototype somewhere..

namespace ImGui
{
    int Curve(const char *label, const ImVec2& size, int maxpoints, ImVec2 *points);
    float CurveValue(float p, int maxpoints, const ImVec2 *points);
    float CurveValueSmooth(float p, int maxpoints, const ImVec2 *points);
};

*/
/*
    Example of use:

    ImVec2 foo[10];
    ...
    foo[0].x = -1; // init data so editor knows to take it from here
    ...
    if (ImGui::Curve("Das editor", ImVec2(600, 200), 10, foo))
    {
        // curve changed
    }
    ...
    float value_you_care_about = ImGui::CurveValue(0.7f, 10, foo); // calculate value at position 0.7
*/


namespace ImGui
{
    // [src] http://iquilezles.org/www/articles/minispline/minispline.htm
    // key format (for dim == 1) is (t0,x0,t1,x1 ...)
    // key format (for dim == 2) is (t0,x0,y0,t1,x1,y1 ...)
    // key format (for dim == 3) is (t0,x0,y0,z0,t1,x1,y1,z1 ...)
    void spline(const float* key, int num, int dim, float t, float* v);

    float CurveValueSmooth(float p, const std::vector<float2>& points);

    float CurveValue(float p, const std::vector<float2>& points);

    int Curve(const char* label, const ImVec2& size, const int maxpoints, std::vector<float2>& points);

};
