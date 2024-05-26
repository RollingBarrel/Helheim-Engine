#ifndef ARCHIVE_H
#define ARCHIVE_H

#undef max
#undef min
#define NOMINMAX
#include "rapidjson/document.h"
#include "rapidjson/allocators.h"
#include <string>
#include "Globals.h"

class JsonObject;
class ENGINE_API JsonArray {
public:
    JsonArray() = delete;
    JsonArray(const rapidjson::Value::Array& arr, rapidjson::MemoryPoolAllocator<>& allocator);
    JsonArray(const JsonArray& other);
    ~JsonArray() {}
    inline void Reserve(unsigned int size);
    inline unsigned int Size() const;
    inline unsigned int Capacity() const;
    inline void PushBackBool(bool value);
    inline void PushBackInt(int value);
    inline void PushBackFloat(float value);
    inline void PushBackString(const char* value);
    inline JsonArray PushBackNewArray();
    inline JsonObject PushBackNewObject();
    inline void PopBack();

    inline bool GetBool(unsigned int idx) const;
    inline int GetInt(unsigned int idx) const;
    inline float GetFloat(unsigned int idx) const;
    inline std::string GetString(unsigned int idx) const;
    inline JsonObject GetJsonObject(unsigned int idx);
    inline JsonArray GetJsonArray(unsigned int idx);

private:
    rapidjson::Value::Array mArray;
    rapidjson::MemoryPoolAllocator<>& mAllocator;

    friend class JsonObject;
};

class ENGINE_API JsonObject {
public:
    JsonObject() = delete;
    JsonObject(const rapidjson::Value::Object& obj, rapidjson::MemoryPoolAllocator<>& allocator);
    JsonObject(const JsonObject& other);
    ~JsonObject() {}

    inline void AddBool(const char* key, bool value);
    inline void AddInt(const char* key, int value);
    void AddInts(const char* key, const int* array, unsigned int numInts);
    inline void AddFloat(const char* key, float value);
    void AddFloats(const char* key, const float* floats, unsigned int numFloats);
    inline void AddString(const char* key, const char* value);
    inline JsonObject AddNewJsonObject(const char* key);
    inline JsonArray AddNewJsonArray(const char* key);

    inline bool GetBool(const char* key) const;
    inline int GetInt(const char* key) const;
    unsigned int GetInts(const char* key, int* fillInts) const;
    inline float GetFloat(const char* key) const;
    unsigned int GetFloats(const char* key, float* fillFloats) const;
    inline std::string GetString(const char* key) const;
    inline JsonObject GetJsonObject(const char* key);
    inline JsonArray GetJsonArray(const char* key);
    bool HasMember(const char* key) const;

private:
    rapidjson::Value::Object mObject;
    rapidjson::MemoryPoolAllocator<>& mAllocator;

    friend class JsonArray;
};

class ENGINE_API Archive
{
public:
    Archive();
    Archive(const char* json);
    ~Archive();
    Archive(const Archive& other);
    Archive(Archive&& other) noexcept;

    Archive& operator=(const Archive& other);
    Archive& operator=(Archive&& other) noexcept;

    JsonObject GetRootObject();

    std::string Serialize() const;

private:
    rapidjson::Document mDocument;
};

#endif // ARCHIVE_H