#ifndef ARCHIVE_H
#define ARCHIVE_H

#undef max
#undef min
#define NOMINMAX
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <vector>
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/Quat.h"

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

    void AddInt(const std::string& key, int value);
    void AddString(const std::string& key, const std::string& value);
    void AddFloat(const std::string& key, float value);
    void AddBool(const std::string& key, bool value);
    void AddIntArray(const std::string& key, const std::vector<unsigned int>& array);
    void AddFloat3(const std::string& key, const float3& vector);
    void AddFloat4x4(const std::string& key, const float4x4& matrix);
    void AddQuat(const std::string& key, const Quat& quat);
    void AddObject(const std::string& key, const Archive& value);

    int GetInt(const std::string& key) const;
    std::string GetString(const std::string& key) const;
    float GetFloat(const std::string& key) const;
    bool GetBool(const std::string& key) const;
    std::vector<Archive> GetArray(const std::string& key) const;
    float3 GetFloat3(const std::string& key) const;
    float4x4 GetFloat4x4(const std::string& key) const;
    Quat GetQuat(const std::string& key) const;
    Archive GetObject(const std::string& key) const;

    void CopyFrom(const rapidjson::Value& value);
    void CopyFrom(const Archive& other);

    std::string Serialize() const;

private:
    std::unique_ptr<rapidjson::Document> mDocument;
};

#endif // ARCHIVE_H