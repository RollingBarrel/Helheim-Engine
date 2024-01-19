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
	std::vector<unsigned int> mUid;
	std::string mLocalPath;
	std::string mLibPath;
	PathNode* mParent;
	//TODO Resource
};

class ProjectPanel : public Panel
{
public:
	ProjectPanel();
	~ProjectPanel();

	void Draw(int windowFlags) override;

private:

	std::vector<AssetDisplay*> assets;

	PathNode* root = nullptr; 
};

