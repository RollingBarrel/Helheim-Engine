#include "archive.h"

Archive::Archive() : mDocument(std::make_unique<rapidjson::Document>()) {
    mDocument->SetObject();
}

Archive::~Archive() {}

void Archive::AddInt(const std::string& key, int value)
{
    rapidjson::Value jsonKey(rapidjson::kStringType);
    jsonKey.SetString(key.c_str(), key.length(), mDocument->GetAllocator());

    rapidjson::Value jsonValue(rapidjson::kNumberType);
    jsonValue.SetInt(value);

    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

void Archive::AddString(const std::string& key, const std::string& value)
{
    rapidjson::Value jsonKey(rapidjson::kStringType);
    jsonKey.SetString(key.c_str(), key.length(), mDocument->GetAllocator());

    rapidjson::Value jsonValue(rapidjson::kStringType);
    jsonValue.SetString(value.c_str(), value.length(), mDocument->GetAllocator());

    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

void Archive::AddFloat(const std::string& key, float value)
{
    rapidjson::Value jsonKey(rapidjson::kStringType);
    jsonKey.SetString(key.c_str(), key.length(), mDocument->GetAllocator());

    rapidjson::Value jsonValue(rapidjson::kNumberType);
    jsonValue.SetFloat(value);

    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

void Archive::AddBool(const std::string& key, bool value)
{
    rapidjson::Value jsonKey(rapidjson::kStringType);
    jsonKey.SetString(key.c_str(), key.length(), mDocument->GetAllocator());

    rapidjson::Value jsonValue(rapidjson::kFalseType);
    jsonValue.SetBool(value);

    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

void Archive::AddIntArray(const std::string& key, const std::vector<unsigned int>& array) {
    rapidjson::Value jsonKey(key.c_str(), mDocument->GetAllocator());
    rapidjson::Value jsonArray(rapidjson::kArrayType);
    for (const auto& item : array) {
        rapidjson::Value jsonItem(rapidjson::kNumberType);
        jsonItem.SetInt(item);
        jsonArray.PushBack(jsonItem, mDocument->GetAllocator());
    }
    mDocument->AddMember(jsonKey, jsonArray, mDocument->GetAllocator());
}

void Archive::AddFloat3(const std::string& key, const float3& vector)
{
    rapidjson::Value jsonKey(rapidjson::kStringType);
    jsonKey.SetString(key.c_str(), key.length(), mDocument->GetAllocator());

    rapidjson::Value jsonValue(rapidjson::kObjectType);
    jsonValue.AddMember("x", vector.x, mDocument->GetAllocator());
    jsonValue.AddMember("y", vector.y, mDocument->GetAllocator());
    jsonValue.AddMember("z", vector.z, mDocument->GetAllocator());

    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

void Archive::AddFloat4x4(const std::string& key, const float4x4& matrix)
{
    rapidjson::Value jsonKey(rapidjson::kStringType);
    jsonKey.SetString(key.c_str(), key.length(), mDocument->GetAllocator());

    rapidjson::Value jsonValue(rapidjson::kArrayType);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            jsonValue.PushBack(matrix[i][j], mDocument->GetAllocator());
        }
    }

    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

void Archive::AddQuat(const std::string& key, const Quat& quat)
{
    rapidjson::Value jsonKey(rapidjson::kStringType);
    jsonKey.SetString(key.c_str(), key.length(), mDocument->GetAllocator());

    rapidjson::Value jsonValue(rapidjson::kObjectType);
    jsonValue.AddMember("x", quat.x, mDocument->GetAllocator());
    jsonValue.AddMember("y", quat.y, mDocument->GetAllocator());
    jsonValue.AddMember("z", quat.z, mDocument->GetAllocator());
    jsonValue.AddMember("w", quat.w, mDocument->GetAllocator());

    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

void Archive::AddObject(const std::string& key, const Archive& value) {
    rapidjson::Value jsonKey(key.c_str(), mDocument->GetAllocator());
    rapidjson::Value jsonValue(rapidjson::kObjectType);
    jsonValue.CopyFrom(*value.mDocument, mDocument->GetAllocator());
    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

/*void Archive::AddObjectWithId(const unsigned int objectId, const Archive& objectArchive) {
    // Convert the object ID to string
    std::string key = std::to_string(objectId);

    // Add the object to the archive
    rapidjson::Value jsonKey(key.c_str(), mDocument->GetAllocator());
    rapidjson::Value jsonObject(rapidjson::kObjectType);
    jsonObject.CopyFrom(*objectArchive.mDocument, mDocument->GetAllocator());
    mDocument->AddMember(jsonKey, jsonObject, mDocument->GetAllocator());
}*/

int Archive::GetInt(const std::string& key) const
{
    auto it = mDocument->FindMember(key.c_str());
    if (it != mDocument->MemberEnd() && it->value.IsInt()) {
        return it->value.GetInt();
    }
    // Handle error or return a default value
    return 0;
}

std::string Archive::GetString(const std::string& key) const
{
    auto it = mDocument->FindMember(key.c_str());
    if (it != mDocument->MemberEnd() && it->value.IsString())
    {
        return it->value.GetString();
    }
    // Handle error or return a default value
    return "";
}

float Archive::GetFloat(const std::string& key) const
{
    auto it = mDocument->FindMember(key.c_str());
    if (it != mDocument->MemberEnd() && it->value.IsNumber())
    {
        return it->value.GetFloat();
    }
    // Handle error or return a default value
    return 0.0f;
}

bool Archive::GetBool(const std::string& key) const
{
    auto it = mDocument->FindMember(key.c_str());
    if (it != mDocument->MemberEnd() && it->value.IsBool())
    {
        return it->value.GetBool();
    }
    // Handle error or return a default value
    return false;
}

std::vector<Archive> Archive::GetArray(const std::string& key) const
{
    auto it = mDocument->FindMember(key.c_str());
    if (it != mDocument->MemberEnd() && it->value.IsArray()) {
        const auto& array = it->value.GetArray();
        std::vector<Archive> result;
        for (const auto& element : array) {
            Archive elementArchive;
            elementArchive.CopyFrom(element);
            result.push_back(elementArchive);
        }
        return result;
    }
    // Handle error or return an empty vector
    return std::vector<Archive>();
}

float3 Archive::GetFloat3(const std::string& key) const
{
    auto it = mDocument->FindMember(key.c_str());
    if (it != mDocument->MemberEnd() && it->value.IsObject())
    {
        const auto& object = it->value.GetObject();
        float x = object["x"].GetFloat();
        float y = object["y"].GetFloat();
        float z = object["z"].GetFloat();
        return float3(x, y, z);
    }
    // Handle error or return a default value
    return float3(0.0f, 0.0f, 0.0f);
}

float4x4 Archive::GetFloat4x4(const std::string& key) const
{
    auto it = mDocument->FindMember(key.c_str());
    if (it != mDocument->MemberEnd() && it->value.IsArray())
    {
        const auto& array = it->value.GetArray();
        float4x4 result;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result[i][j] = array[i * 4 + j].GetFloat();
            }
        }

        return result;
    }
    // Handle error or return a default value
    return float4x4::identity;
}

Quat Archive::GetQuat(const std::string& key) const
{
    auto it = mDocument->FindMember(key.c_str());
    if (it != mDocument->MemberEnd() && it->value.IsObject())
    {
        const auto& object = it->value.GetObject();
        float x = object["x"].GetFloat();
        float y = object["y"].GetFloat();
        float z = object["z"].GetFloat();
        float w = object["w"].GetFloat();
        return Quat(x, y, z, w);
    }
    // Handle error or return a default value
    return Quat::identity;
}

Archive Archive::GetObject(const std::string& key) const
{
    auto it = mDocument->FindMember(key.c_str());
    if (it != mDocument->MemberEnd() && it->value.IsObject()) {
        Archive objectArchive;
        objectArchive.CopyFrom(it->value);
        return objectArchive;
    }
    // Handle error or return an empty Archive
    return Archive();
}

void Archive::CopyFrom(const rapidjson::Value& value)
{
    mDocument->CopyFrom(value, mDocument->GetAllocator());
}

void Archive::CopyFrom(const Archive& other) {
    rapidjson::Document tempDocument;
    tempDocument.CopyFrom(*other.mDocument, tempDocument.GetAllocator());
    mDocument->Swap(tempDocument);
}

std::string Archive::Serialize() const
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    mDocument->Accept(writer);
    return buffer.GetString();
}