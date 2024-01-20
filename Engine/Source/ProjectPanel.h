#pragma once
#include "Panel.h"
#define PROJECTPANEL "Project##"

#include <vector>

struct PathNode;

struct AssetDisplay
{
	AssetDisplay(const char* name, PathNode* parent);
	const char* mName;
	std::vector<unsigned int> mUid;
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

};

