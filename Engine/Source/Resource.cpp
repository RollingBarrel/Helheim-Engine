#include "Resource.h"

Resource::Resource()
{
	mUID = 0;
	mType = Type::Unknown;
	mReferenceCount = 0;
}

Resource::Resource(unsigned int uid)
{
	mUID = uid;
	mType = Type::Unknown;
	mReferenceCount = 0;
}

Resource::~Resource()
{
}
