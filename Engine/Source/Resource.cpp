#define _CRT_SECURE_NO_WARNINGS
#include "Resource.h"


Resource::Resource(unsigned int uid, Type type) : mUID(uid), mType(type), mReferenceCount(0)
{
}

Resource::~Resource()
{
}