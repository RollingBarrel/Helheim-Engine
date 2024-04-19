#include "archive.h"

Archive::Archive() : mDocument(std::make_unique<rapidjson::Document>()) {
    mDocument->SetObject();
}

Archive::~Archive() {}

void Archive::AddInt(const char* key, int value)
{
    rapidjson::Value jsonKey(rapidjson::kStringType);
    jsonKey.SetString(key, strlen(key), mDocument->GetAllocator());

    rapidjson::Value jsonValue(rapidjson::kNumberType);
    jsonValue.SetInt(value);

    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

void Archive::AddString(const char* key, const char* value)
{
    rapidjson::Value jsonKey(rapidjson::kStringType);
    jsonKey.SetString(key, strlen(key), mDocument->GetAllocator());

    rapidjson::Value jsonValue(rapidjson::kStringType);
    jsonValue.SetString(value, strlen(value), mDocument->GetAllocator());

    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

void Archive::AddFloat(const char* key, float value)
{
    rapidjson::Value jsonKey(rapidjson::kStringType);
    jsonKey.SetString(key, strlen(key), mDocument->GetAllocator());

    rapidjson::Value jsonValue(rapidjson::kNumberType);
    jsonValue.SetFloat(value);

    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

void Archive::AddBool(const char* key, bool value)
{
    rapidjson::Value jsonKey(rapidjson::kStringType);
    jsonKey.SetString(key, strlen(key), mDocument->GetAllocator());

    rapidjson::Value jsonValue(rapidjson::kFalseType);
    jsonValue.SetBool(value);

    mDocument->AddMember(jsonKey, jsonValue, mDocument->GetAllocator());
}

void Archive::AddIntArray(const char* key, const std::vector<unsigned int>& array) {
    rapidjson::Value jsonKey(key, mDocument->GetAllocator());
    rapidjson::Value jsonArray(rapidjson::kArrayType);
    for (const auto& item : array) {
        rapidjson::Value jsonItem(rapidjson::kNumberType);
        jsonItem.SetInt(item);
        jsonArray.PushBack(jsonItem, mDocument->GetAllocator());
    }
    mDocument->AddMember(jsonKey, jsonArray, mDocument->GetAllocator());
}

void Archive::AddFloatArray(const char* key, const std::vector<float>& array)
{
    rapidjson::Value jsonKey(key, mDocument->GetAllocator());
    rapidjson::Value jsonArray(rapidjson::kArrayType);
    for (const auto& item : array) {
        rapidjson::Value jsonItem(rapidjson::kNumberType);
        jsonItem.SetFloat(item);
        jsonArray.PushBack(jsonItem, mDocument->GetAllocator());
    }
    mDocument->AddMember(jsonKey, jsonArray, mDocument->GetAllocator());
}

void Archive::AddObjectArray(const char* key, const std::vector<Archive>& array) {
    rapidjson::Value jsonKey(key, mDocument->GetAllocator());
    rapidjson::Value jsonArray(rapidjson::kArrayType);

    for (const auto& item : array) {
        rapidjson::Value jsonItem(rapidjson::kObjectType);
        jsonItem.CopyFrom(*item.mDocument, mDocument->GetAllocator());
        jsonArray.PushBack(jsonItem, mDocument->GetAllocator());
    }

    mDocument->AddMember(jsonKey, jsonArray, mDocument->GetAllocator());
}

std::vector<Archive> Archive::GetObjectArray(const char* key) const {
    auto it = mDocument->FindMember(key);
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

void Archive::AddFloat2(const char* key, const float2& vector)
{
    rapidjson::Value jsonKey(key, mDocument->GetAllocator());
    rapidjson::Value jsonArray(rapidjson::kArrayType);

    jsonArray.PushBack(vector.x, mDocument->GetAllocator());
    jsonArray.PushBack(vector.y, mDocument->GetAllocator());

    mDocument->AddMember(jsonKey, jsonArray, mDocument->GetAllocator());
}

void Archive::AddFloat3(const char* key, const float3& vector)
{
    rapidjson::Value jsonKey(key, mDocument->GetAllocator());
    rapidjson::Value jsonArray(rapidjson::kArrayType);

    jsonArray.PushBack(vector.x, mDocument->GetAllocator());
    jsonArray.PushBack(vector.y, mDocument->GetAllocator());
    jsonArray.PushBack(vector.z, mDocument->GetAllocator());

    mDocument->AddMember(jsonKey, jsonArray, mDocument->GetAllocator());
}

void Archive::AddFloat4(const char* key, const float vector[4])
{
    rapidjson::Value jsonKey(key, mDocument->GetAllocator());
    rapidjson::Value jsonArray(rapidjson::kArrayType);

    jsonArray.PushBack(vector[0], mDocument->GetAllocator());
    jsonArray.PushBack(vector[1], mDocument->GetAllocator());
    jsonArray.PushBack(vector[2], mDocument->GetAllocator());
    jsonArray.PushBack(vector[3], mDocument->GetAllocator());

    mDocument->AddMember(jsonKey, jsonArray, mDocument->GetAllocator());
}

void Archive::AddFloat4x4(const char* key, const float4x4& matrix)
{
    rapidjson::Value jsonKey(key, mDocument->GetAllocator());
    rapidjson::Value jsonArray(rapidjson::kArrayType);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            jsonArray.PushBack(matrix[i][j], mDocument->GetAllocator());
        }
    }

    mDocument->AddMember(jsonKey, jsonArray, mDocument->GetAllocator());
}

void Archive::AddQuat(const char* key, const Quat& quat)
{
    rapidjson::Value jsonKey(key, mDocument->GetAllocator());
    rapidjson::Value jsonArray(rapidjson::kArrayType);

    jsonArray.PushBack(quat.x, mDocument->GetAllocator());
    jsonArray.PushBack(quat.y, mDocument->GetAllocator());
    jsonArray.PushBack(quat.z, mDocument->GetAllocator());
    jsonArray.PushBack(quat.w, mDocument->GetAllocator());

    mDocument->AddMember(jsonKey, jsonArray, mDocument->GetAllocator());
}

void Archive::AddObject(const char* key, const Archive& value) {
    rapidjson::Value jsonKey(key, mDocument->GetAllocator());
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

int Archive::GetInt(const char* key) const
{
    auto it = mDocument->FindMember(key);
    if (it != mDocument->MemberEnd() && it->value.IsInt()) {
        return it->value.GetInt();
    }
    // Handle error or return a default value
    return 0;
}

std::string Archive::GetString(const char* key) const
{
    auto it = mDocument->FindMember(key);
    if (it != mDocument->MemberEnd() && it->value.IsString())
    {
        return it->value.GetString();
    }
    // Handle error or return a default value
    return "";
}

float Archive::GetFloat(const char* key) const
{
    auto it = mDocument->FindMember(key);
    if (it != mDocument->MemberEnd() && it->value.IsNumber())
    {
        return it->value.GetFloat();
    }
    // Handle error or return a default value
    return 0.0f;
}

bool Archive::GetBool(const char* key) const
{
    auto it = mDocument->FindMember(key);
    if (it != mDocument->MemberEnd() && it->value.IsBool())
    {
        return it->value.GetBool();
    }
    // Handle error or return a default value
    return false;
}

std::vector<Archive> Archive::GetArray(const char* key) const
{
    auto it = mDocument->FindMember(key);
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

float2 Archive::GetFloat2(const char* key) const
{
    auto it = mDocument->FindMember(key);
    if (it != mDocument->MemberEnd() && it->value.IsObject())
    {
        const auto& object = it->value.GetObject();
        float x = object["x"].GetFloat();
        float y = object["y"].GetFloat();
        return float2(x, y);
    }
    // Handle error or return a default value
    return float2(0.0f, 0.0f);
}

float3 Archive::GetFloat3(const char* key) const
{
    auto it = mDocument->FindMember(key);
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

float4 Archive::GetFloat4(const char* key) const
{
    auto it = mDocument->FindMember(key);
    if (it != mDocument->MemberEnd() && it->value.IsObject())
    {
        const auto& object = it->value.GetObject();
        float x = object["x"].GetFloat();
        float y = object["y"].GetFloat();
        float z = object["z"].GetFloat();
        float w = object["w"].GetFloat();
        return float4(x, y, z, w);
    }
    // Handle error or return a default value
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}


float4x4 Archive::GetFloat4x4(const char* key) const
{
    auto it = mDocument->FindMember(key);
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

Quat Archive::GetQuat(const char* key) const
{
    auto it = mDocument->FindMember(key);
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

Archive Archive::GetObject(const char* key) const
{
    auto it = mDocument->FindMember(key);
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
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    mDocument->Accept(writer);
    return buffer.GetString();
}