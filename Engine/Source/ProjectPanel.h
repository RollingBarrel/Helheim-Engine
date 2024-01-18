#pragma once
#include "Panel.h"
#define PROJECTPANEL "Project##"

#include <vector>
#include <string>

struct PathNode
{
	const char* mName;
	PathNode* mParent;
	std::vector<PathNode*> mChildren;
};

struct AssetDisplay
{
	const char* mName;
	std::string mLocalPath;
	std::string mLibPath;
	//TODO Resource
};

class ProjectPanel : public Panel
{
public:
	ProjectPanel();
	~ProjectPanel();

	void Draw(int windowFlags) override;

private:
	std::vector<std::string> mFiles;
	std::vector<std::string> mDirectories;

	PathNode* root = nullptr; 
};

