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

void BezierCurve::SaveJson(Archive& archive) const
{
    archive.AddFloat("Lineal", mLineal);
    archive.AddBool("isCurve", mIsCurve);
    archive.AddFloat4("Curve", mCurve.ptr());
    archive.AddFloat("Factor", mCurveFactor);
}

void BezierCurve::LoadJson(const rapidjson::Value& data)
{
    if (data.HasMember("Lineal") && data["Lineal"].IsFloat())
    {
        mLineal = data["Lineal"].GetFloat();
    }
    if (data.HasMember("Factor") && data["Factor"].IsFloat())
    {
        mCurveFactor = data["Factor"].GetFloat();
    }
    if (data.HasMember("isCurve") && data["isCurve"].IsBool())
    {
        mIsCurve = data["isCurve"].GetBool();
    }
    if (data.HasMember("Curve") && data["Curve"].IsArray())
    {
        const rapidjson::Value& values = data["Curve"];
        float x{ 0.0f }, y{ 0.0f }, z{ 0.0f }, w{ 0.0f };
        if (values.Size() == 4 && values[0].IsFloat() && values[1].IsFloat() && values[2].IsFloat() && values[3].IsFloat())
        {
            x = values[0].GetFloat();
            y = values[1].GetFloat();
            z = values[2].GetFloat();
            w = values[3].GetFloat();
        }

        mCurve = float4(x, y, z, w);
    }
}