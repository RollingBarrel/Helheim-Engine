#ifndef ARCHIVE_H
#define ARCHIVE_H

#undef max
#undef min
#define NOMINMAX
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include <vector>
#include "Math/Quat.h"
#include "Math/float4x4.h"
#include "Math/float2.h"
#include "Math/float3.h"
#include "Math/float4.h"

class Archive
{
public:
    Archive();
    ~Archive();

    // Copy constructor
    Archive(const Archive& other) : mDocument(std::make_unique<rapidjson::Document>()) {
        mDocument->CopyFrom(*other.mDocument, mDocument->GetAllocator());
    }

    // Move constructor
    Archive(Archive&& other) noexcept : mDocument(std::move(other.mDocument)) {
        other.mDocument = nullptr;
    }

    // Copy assignment operator
    Archive& operator=(const Archive& other) {
        if (this != &other) {
            mDocument = std::make_unique<rapidjson::Document>();
            mDocument->CopyFrom(*other.mDocument, mDocument->GetAllocator());
        }
        return *this;
    }

    // Move assignment operator
    Archive& operator=(Archive&& other) noexcept {
        if (this != &other) {
            mDocument = std::move(other.mDocument);
            other.mDocument = nullptr;
        }
        return *this;
    }

    void AddInt(const char* key, int value);
    void AddString(const char* key, const char* value);
    void AddFloat(const char* key, float value);
    void AddBool(const char* key, bool value);
    void AddIntArray(const char* key, const std::vector<unsigned int>& array);
    void AddFloatArray(const char* key, const std::vector<float>& array);
    void AddStringArray(const char* key, const std::vector<const char*>& array);
    void AddFloat2(const char* key, const float2& vector);
    void AddFloat3(const char* key, const float3& vector);
    void AddFloat4(const char* key, const float vector[4]);
    void AddFloat4x4(const char* key, const float4x4& matrix);
    void AddQuat(const char* key, const Quat& quat);
    void AddObject(const char* key, const Archive& value);
    void AddObjectArray(const char* key, const std::vector<Archive>& array);
    std::vector<Archive> GetObjectArray(const char* key) const;

    int GetInt(const char* key) const;
    std::string GetString(const char* key) const;
    float GetFloat(const char* key) const;
    bool GetBool(const char* key) const;
    std::vector<Archive> GetArray(const char* key) const;
    float2 GetFloat2(const char* key) const;
    float3 GetFloat3(const char* key) const;
    float4 GetFloat4(const char* key) const;
    float4x4 GetFloat4x4(const char* key) const;
    Quat GetQuat(const char* key) const;
    Archive GetObject(const char* key) const;

    void CopyFrom(const rapidjson::Value& value);
    void CopyFrom(const Archive& other);

    std::string Serialize() const;

private:
    std::unique_ptr<rapidjson::Document> mDocument;
};

#endif // ARCHIVE_H