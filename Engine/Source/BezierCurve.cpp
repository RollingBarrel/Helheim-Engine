#include "BezierCurve.h"

BezierCurve::BezierCurve()
{
}

float BezierCurve::GetValue(float dt, float initialValue) const
{
    return mIsCurve ? initialValue + (CurveValue(dt) * mFactor) : initialValue;
}

float BezierCurve::CalculateInitialValue()
{
    CalculateRandomValue();
    return mInitialValue;
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

float BezierCurve::CalculateRandomValue()
{
    if (mIsValueRandom)
    {
        float random_value = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        mInitialValue = random_value * (mMaxValue - mValue) + mValue;
    }
    else
    {
        mInitialValue = mValue;
    }
    return mInitialValue;
}

void BezierCurve::Save(JsonObject& obj) const
{
    //TODO: Test
    obj.AddBool("IsValueRandom", mIsValueRandom);
    obj.AddFloat("Value", mValue);
    obj.AddFloat("MaxValue", mMaxValue);
    obj.AddBool("IsCurve", mIsCurve);
    obj.AddFloat("Factor", mFactor);

    JsonArray curvePoint = obj.AddNewJsonArray("CurvePoints");

    for(unsigned int i = 0; mPoints.size(); ++i)
    {
        JsonObject point = curvePoint.PushBackNewObject();
        point.AddFloats("Point", mPoints[i].ptr(), 2);
    }
}

void BezierCurve::Load(const JsonObject& data)
{
     //TODO: Test   
    mIsValueRandom = data.GetBool("IsValueRandom");
    mValue = data.GetFloat("Value");
    mMaxValue = data.GetFloat("MaxValue");
    mFactor = data.GetFloat("Factor");
    mIsCurve = data.GetBool("IsCurve");

    JsonArray curvePoint = data.GetJsonArray("CurvePoints");

    for (int i = 0; i < curvePoint.Size(); ++i)
    {
        JsonObject pointObj = curvePoint.GetJsonObject(i);
        float point[2];
        pointObj.GetFloats("Point", point);
        mPoints.push_back(float2(point));    
    }
}