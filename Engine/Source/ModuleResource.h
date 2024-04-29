#ifndef _MODULE_RESOURCE_H_
#define _MODULE_RESOURCE_H_

#include "Module.h"
#include "Resource.h"
#include <map>
#include <unordered_map>

class ENGINE_API ModuleResource : public Module
{
public:
	ModuleResource() {}
	virtual ~ModuleResource() {};

	unsigned int Find(const char* assetsFile) const;

	Resource* RequestResource(const char* assetsPath);
	Resource* RequestResource(unsigned int uid, Resource::Type type);
	void ReleaseResource(unsigned int uid);

protected:

	std::map<unsigned int, Resource*> mResources;
};

#endif //_MODULE_RESOURCE_H_