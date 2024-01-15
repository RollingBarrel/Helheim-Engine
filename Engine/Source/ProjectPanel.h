#pragma once
#include "Panel.h"
#define PROJECTPANEL "Project##"

#include <vector>
#include <string>

class ProjectPanel : public Panel
{
public:
	ProjectPanel();
	~ProjectPanel();

	void Draw(int windowFlags) override;

private:
	std::vector<std::string> mFiles;
	std::vector<std::string> mDirectories;
;
};

