#include "BezierCurve.h"

BezierCurve::BezierCurve()
{
}

float BezierCurve::CalculateValue(float dt, const float initialValue) const
{
    return mIsCurve ? initialValue + (CurveValue(dt) * mFactor) : initialValue;
}

void BezierCurve::spline(const float* key, int num, int dim, float t, float* v) const
{
    static signed char coefs[16] = 
    {
        -1, 2,-1, 0,
         3,-5, 0, 2,
        -3, 4, 1, 0,
         1,-1, 0, 0 
    };

    const int size = dim + 1;
    // find key
    int k = 0; while (key[k * size] < t) k++;
    // interpolant
    const float h = (t - key[(k - 1) * size]) / (key[k * size] - key[(k - 1) * size]);
    // init result
    for (int i = 0; i < dim; i++) v[i] = 0.0f;
    // add basis functions
    for (int i = 0; i < 4; i++)
    {
        int kn = k + i - 2; if (kn < 0) kn = 0; else if (kn > (num - 1)) kn = num - 1;
        const signed char* co = coefs + 4 * i;
        const float b = 0.5f * (((co[0] * h + co[1]) * h + co[2]) * h + co[3]);
        for (int j = 0; j < dim; j++) v[j] += b * key[kn * size + j + 1];
    }
}

float BezierCurve::CurveValueSmooth(float p) const
{
    if (mPoints.size() < 2 || mPoints.empty())
        return 0;
    if (p < 0) return mPoints[0].y;

    float* input = new float[mPoints.size() * 2];
    float output[4];

    for (int i = 0; i < mPoints.size(); ++i) 
    {
        input[i * 2 + 0] = mPoints[i].x;
        input[i * 2 + 1] = mPoints[i].y;
    }

    spline(input, mPoints.size(), 1, p, output);

    delete[] input;
    return output[0];
}

float BezierCurve::CurveValue(float p) const
{
    if (mPoints.size() < 2 || mPoints.empty())
        return 0;
    if (p < 0) return mPoints[0].y;

    int left = 0;
    while (left < mPoints.size() && mPoints[left].x < p && mPoints[left].x != -1) left++;
    if (left) left--;

    if (left == mPoints.size() - 1)
        return mPoints[mPoints.size() - 1].y;

    float d = (p - mPoints[left].x) / (mPoints[left + 1].x - mPoints[left].x);

    return mPoints[left].y + (mPoints[left + 1].y - mPoints[left].y) * d;
}

void BezierCurve::Save(JsonObject& obj) const
{
    JsonObject value = obj.AddNewJsonObject("Value");
    mValue.Save(value);
    obj.AddBool("IsCurve", mIsCurve);
    obj.AddFloat("Factor", mFactor);

    JsonArray curvePoint = obj.AddNewJsonArray("CurvePoints");

    for(unsigned int i = 0; i < mPoints.size(); ++i)
    {
        JsonObject point = curvePoint.PushBackNewObject();
        point.AddFloats("Point", mPoints[i].ptr(), 2);
    }
}

void BezierCurve::Load(const JsonObject& data)
{
    if (data.HasMember("Factor")) mFactor = data.GetFloat("Factor");
    if (data.HasMember("IsCurve")) mIsCurve = data.GetBool("IsCurve");

    if (data.HasMember("CurvePoints")) {
        JsonArray curvePoint = data.GetJsonArray("CurvePoints");

        for (unsigned int i = 0; i < curvePoint.Size(); ++i)
        {
            JsonObject pointObj = curvePoint.GetJsonObject(i);
            if (data.HasMember("Point"))
            {
                float point[2];
                pointObj.GetFloats("Point", point);
                mPoints.push_back(float2(point));
            }
        }
    }
    if (data.HasMember("Value")) 
    {
        JsonObject value = data.GetJsonObject("Value");
        mValue.Load(value);
    }
}