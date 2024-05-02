#pragma once

class ResourceModel;

namespace Importer
{
	namespace Model
	{
		ResourceModel* Import(const char* filePath, unsigned int uid, bool modifyAssets = true);
	}
}

