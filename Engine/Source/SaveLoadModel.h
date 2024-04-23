#pragma once

class ResourceModel;

namespace Importer
{
	namespace Model
	{
		void Save(const ResourceModel* ourModel, unsigned int& size);
		ResourceModel* Load(const char* fileName, unsigned int uid);
	};
};