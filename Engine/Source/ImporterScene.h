#pragma once

#include "Importer.h"

class ImporterScene : public Importer
{
	void Import(const char* filePath) const override;
};

