#pragma once
#include "Panel.h"
#define PROJECTPANEL "Project##"

#include <vector>

struct PathNode;
class Resource;

struct AssetDisplay
{
	AssetDisplay(const char* name, PathNode* parent);
	const char* mName;
	std::vector<unsigned int> mUid;
	PathNode* mParent;
	Resource* mResource;
};

class ProjectPanel : public Panel
{
public:
	ProjectPanel();
	~ProjectPanel();

	void Draw(int windowFlags) override;

private:
	const void DrawAssetsFolder(const PathNode& current) const;
};

