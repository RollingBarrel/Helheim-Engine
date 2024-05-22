#include "Archive.h"
#include "archive.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

Archive::Archive()
{
    mDocument.SetObject();
    JsonObject Object(mDocument.GetObject(), mDocument.GetAllocator());
}
Archive::Archive(const char* json)
{
    mDocument.Parse(json);
}
// Copy constructor
Archive::Archive(const Archive& other)
{
    mDocument.CopyFrom(other.mDocument, mDocument.GetAllocator());
}

Archive::Archive(Archive&& other) noexcept : mDocument(std::move(other.mDocument)) {}

Archive::~Archive() {}

Archive& Archive::operator=(const Archive& other) 
{
    assert(this != &other && "this is the same than other");
    mDocument.CopyFrom(other.mDocument, mDocument.GetAllocator());
    return *this;
}

Archive& Archive::operator=(Archive&& other) noexcept 
{
    assert(this != &other && "other is the same than this");
    mDocument = std::move(other.mDocument);
    return *this;
}

JsonArray::JsonArray(const rapidjson::Value::Array& arr, rapidjson::MemoryPoolAllocator<>& allocator) : mArray(arr), mAllocator(allocator) {}

JsonArray::JsonArray(const JsonArray& other) : mArray(other.mArray), mAllocator(other.mAllocator) {}

void JsonArray::Reserve(unsigned int size)
{
    mArray.Reserve(size, mAllocator);
}

unsigned int JsonArray::Size() const
{
    return mArray.Size();
}

unsigned int JsonArray::Capacity() const
{
    return mArray.Capacity();
}

void JsonArray::PushBackBool(bool value)
{
    mArray.PushBack(rapidjson::Value().SetBool(value), mAllocator);
}

void JsonArray::PushBackInt(int value)
{
    mArray.PushBack(rapidjson::Value().SetInt(value), mAllocator);
}

void JsonArray::PushBackFloat(float value)
{
    mArray.PushBack(rapidjson::Value().SetFloat(value), mAllocator);
}

void JsonArray::PushBackArray(const JsonArray& arr)
{
    mArray.PushBack(arr.mArray, mAllocator);
}

void JsonArray::PushBackObject(const JsonObject& obj)
{
    mArray.PushBack(obj.mObject, mAllocator);
}

void JsonArray::PopBack()
{
    mArray.PopBack();
}

JsonObject::JsonObject(const rapidjson::Value::Object& obj, rapidjson::MemoryPoolAllocator<>& allocator): mObject(obj), mAllocator(allocator){}

JsonObject::JsonObject(const JsonObject& other) : mObject(other.mObject), mAllocator(other.mAllocator) {}

void JsonObject::AddInt(const char* key, int value)
{
    mObject.AddMember(rapidjson::Value().SetString(key, mAllocator), rapidjson::Value().SetInt(value), mAllocator);
}

void JsonObject::AddString(const char* key, const char* value)
{
    mObject.AddMember(rapidjson::Value().SetString(key, mAllocator), rapidjson::Value().SetString(value, mAllocator), mAllocator);
}

void JsonObject::AddFloat(const char* key, float value)
{
    mObject.AddMember(rapidjson::Value().SetString(key, mAllocator), rapidjson::Value().SetFloat(value), mAllocator);
}

void JsonObject::AddBool(const char* key, bool value)
{
    mObject.AddMember(rapidjson::Value().SetString(key, mAllocator), rapidjson::Value().SetBool(value), mAllocator);
}

void JsonObject::AddInts(const char* key, const int* array, unsigned int numInts) {
    rapidjson::Value jsonArray(rapidjson::kArrayType);
    jsonArray.Reserve(numInts * sizeof(int), mAllocator);
    for (unsigned int i = 0; i < numInts; ++i) 
    {
        jsonArray.PushBack(array[i], mAllocator);
    }
    mObject.AddMember(rapidjson::Value().SetString(key, mAllocator), jsonArray, mAllocator);
}

void JsonObject::AddJsonArray(const char* key, JsonArray arr)
{
    mObject.AddMember(rapidjson::Value().SetString(key, mAllocator), arr.mArray, mAllocator);
}

void JsonObject::AddJsonObject(const char* key, JsonObject obj)
{
    mObject.AddMember(rapidjson::Value().SetString(key, mAllocator), obj.mObject, mAllocator);
}

void JsonObject::AddFloats(const char* key, const float* floats, unsigned int numFloats)
{
    rapidjson::Value jsonArray(rapidjson::kArrayType);
    jsonArray.Reserve(numFloats * sizeof(float), mAllocator);
    for (int i = 0; i < numFloats; ++i)
    {
        jsonArray.PushBack(floats[i], mAllocator);
    }

    mObject.AddMember(rapidjson::Value().SetString(key, mAllocator), jsonArray, mAllocator);
}

bool JsonObject::GetBool(const char* key) const
{
    assert(mObject.HasMember(key) && "Document does not have this member");
    assert(mObject[key].IsBool() && "This member is not a bool");
    return mObject[key].GetBool();
}

int JsonObject::GetInt(const char* key) const
{
    assert(mObject.HasMember(key) && "Document does not have this member");
    assert(mObject[key].IsInt() && "This member is not an int");
    return mObject[key].GetInt();
}

int* JsonObject::GetInts(const char* key) const
{
    assert(mObject.HasMember(key) && "Document does not have this member");
    assert(mObject[key].IsArray() && "This member is not an array");
    const rapidjson::Value::Array& ints = mObject[key].GetArray();
    rapidjson::SizeType numInts = ints.Size();
    int* ret = new int[numInts];
    for (rapidjson::SizeType i = 0; i < numInts; ++i)
    {
        ret[i] = ints[i].GetInt();
    }
    return ret;
}

unsigned int JsonObject::GetInts(const char* key, int* fillInts) const
{
    assert(mObject.HasMember(key) && "Document does not have this member");
    assert(mObject[key].IsArray() && "This member is not an array");
    const rapidjson::Value::Array& ints = mObject[key].GetArray();
    rapidjson::SizeType numInts = ints.Size();
    for (rapidjson::SizeType i = 0; i < numInts; ++i)
    {
        fillInts[i] = ints[i].GetInt();
    }
    return numInts;
}

float JsonObject::GetFloat(const char* key) const
{
    assert(mObject.HasMember(key) && "Document does not have this member");
    assert(mObject[key].IsFloat() && "This member is not a float");
    return mObject[key].GetFloat();
}

float* JsonObject::GetFloats(const char* key) const
{
    assert(mObject.HasMember(key) && "Document does not have this member");
    assert(mObject[key].IsArray() && "This member is not an array");
    const rapidjson::Value& floats = mObject[key].GetArray();
    rapidjson::SizeType numFloats = floats.Size();
    float* ret = new float[numFloats];
    for (rapidjson::SizeType i = 0; i < numFloats; ++i)
    {
        ret[i] = floats[i].GetFloat();
    }
    return ret;
}

unsigned int JsonObject::GetFloats(const char* key, float* fillFloats) const
{
    assert(mObject.HasMember(key) && "Document does not have this member");
    assert(mObject[key].IsArray() && "This member is not an array");
    const rapidjson::Value::Array& floats = mObject[key].GetArray();
    rapidjson::SizeType numFloats = floats.Size();
    for (rapidjson::SizeType i = 0; i < numFloats; ++i)
    {
        fillFloats[i] = floats[i].GetFloat();
    }
    return numFloats;
}

std::string JsonObject::GetString(const char* key) const
{
    assert(mObject.HasMember(key) && "Document does not have this member");
    assert(mObject[key].IsString() && "This member is not a string");
    return mObject[key].GetString();
}

JsonObject JsonObject::GetJsonObject(const char* key)
{
    assert(mObject.HasMember(key) && "Document does not have this member");
    assert(mObject[key].IsObject() && "This member is not an object");
    return JsonObject(mObject[key].GetObject(), mAllocator);
}

JsonArray JsonObject::GetJsonArray(const char* key)
{
    assert(mObject.HasMember(key) && "Document does not have this member");
    assert(mObject[key].IsArray() && "This member is not an array");
    return JsonArray(mObject[key].GetArray(), mAllocator);
}

bool JsonObject::HasMember(const char* key) const
{
    return mObject.FindMember(key) != mObject.MemberEnd();
}

JsonObject Archive::GetRootObject()
{
    return JsonObject(mDocument.GetObject(), mDocument.GetAllocator());
}

std::string Archive::Serialize() const
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    mDocument.Accept(writer);
    return buffer.GetString();
}

bool Archive::Deserialize(char** buffer) const
{
    rapidjson::ParseResult error = mDocument->Parse(*buffer);
    if (!error)
    {
        return false;
    }

    return true;
}