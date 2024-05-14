#include "Trail.h"

void Trail::SaveJson(Archive& archive) const
{
    archive.AddFloat("Max Life Time", mMaxLifeTime);
    Archive width;
    mWidth.SaveJson(width);
    archive.AddObject("Width", width);
    mGradient.Save(archive);
}

void Trail::LoadJson(const rapidjson::Value& data)
{
    if (data.HasMember("Max Life Time") && data["Max Life Time"].IsFloat())
    {
        mMaxLifeTime = data["Max Life Time"].GetFloat();
    }
    if (data.HasMember("Color Gradient") && data["Color Gradient"].IsArray())
    {
        mGradient.LoadFromJSON(data);
    }
    if (data.HasMember("Width") && data["Width"].IsObject())
    {
        mWidth.LoadJson(data["Width"]);
    }
}