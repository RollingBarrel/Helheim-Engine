#pragma once
#include "Importer.h"

class ImporterModel : public Importer
{
	void Import(const char* filePath) const override;
};

