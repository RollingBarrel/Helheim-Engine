#include "BezierCurve.h"

BezierCurve::BezierCurve()
{
}

float BezierCurve::GetValue(float dt) const
{
    return mIsCurve ? mLineal + (BezierValue(dt, mCurve) * mCurveFactor) : mLineal;
}

float BezierCurve::BezierValue(float dt01, float4 P)
{
    enum { STEPS = 256 };
    float2 Q[4] = { {0, 0}, {P[0], P[1]}, {P[2], P[3]}, {1, 1} };
    float2 results[STEPS + 1];
    BezierCurve::BezierTable<STEPS>(Q, results);
    return results[(int)((dt01 < 0 ? 0 : dt01 > 1 ? 1 : dt01) * STEPS)].y;
}

template <int steps>
void BezierCurve::BezierTable(float2 P[], float2 results[])
{
    static float C[(steps + 1) * 4], * K = 0;
    if (!K)
    {
        K = C;
        for (unsigned step = 0; step <= steps; ++step)
        {
            float t = (float)step / (float)steps;
            C[step * 4 + 0] = (1 - t) * (1 - t) * (1 - t); // * P0
            C[step * 4 + 1] = 3 * (1 - t) * (1 - t) * t;   // * P1
            C[step * 4 + 2] = 3 * (1 - t) * t * t;         // * P2
            C[step * 4 + 3] = t * t * t;                   // * P3
        }
    }
    for (unsigned step = 0; step <= steps; ++step)
    {
        float2 point = {
            K[step * 4 + 0] * P[0].x + K[step * 4 + 1] * P[1].x + K[step * 4 + 2] * P[2].x + K[step * 4 + 3] * P[3].x,
            K[step * 4 + 0] * P[0].y + K[step * 4 + 1] * P[1].y + K[step * 4 + 2] * P[2].y + K[step * 4 + 3] * P[3].y };
        results[step] = point;
    }
}

void BezierCurve::Save(JsonObject& obj) const
{
    obj.AddFloat("Lineal", mLineal);
    obj.AddFloat("Factor", mCurveFactor);
    obj.AddBool("IsCurve", mIsCurve);
    obj.AddFloats("Curve", mCurve.ptr(), 4);
}

void BezierCurve::Load(const JsonObject& data)
{
    mLineal = data.GetFloat("Lineal");
    mIsCurve = data.GetBool("IsCurve");
    mCurveFactor = data.GetFloat("Factor");

    float curve[4];
    data.GetFloats("Curve", curve);
    mCurve = float4(curve);
}