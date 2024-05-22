#ifndef ARCHIVE_H
#define ARCHIVE_H

#undef max
#undef min
#define NOMINMAX
#include "rapidjson/document.h"
#include "rapidjson/allocators.h"
#include <string>

class JsonObject;
class JsonArray {
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
    inline void PushBackArray(const JsonArray& arr);
    inline void PushBackObject(const JsonObject& obj);
    inline void PopBack();

private:
    rapidjson::Value::Array mArray;
    rapidjson::MemoryPoolAllocator<>& mAllocator;

    friend class JsonObject;
};

class JsonObject {
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
    inline void AddJsonObject(const char* key, JsonObject obj);
    inline void AddJsonArray(const char* key, JsonArray arr);

    inline bool GetBool(const char* key) const;
    inline int GetInt(const char* key) const;
    //Delete the memory returned in this pointer !!!
    int* GetInts(const char* key) const;
    unsigned int GetInts(const char* key, int* fillInts) const;
    inline float GetFloat(const char* key) const;
    //Delete the memory returned in this pointer !!!
    float* GetFloats(const char* key) const;
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

class Archive
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
    bool Deserialize(char** buffer) const;

    rapidjson::Document* GetDocument() { return mDocument.get(); }

private:
    rapidjson::Document mDocument;
};

#endif // ARCHIVE_H