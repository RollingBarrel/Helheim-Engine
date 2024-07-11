#pragma once
class ResourceIBL;
#include <string>

namespace Importer
{
	namespace IBL
	{
		ResourceIBL* Import(const char* hdrTexPath, unsigned int uid);
	}
}

