#pragma once
#include "Panel.h"
#define PROJECTPANEL "Project##"

#include <vector>

struct PathNode;
class Resource;

struct AssetDisplay
{
	AssetDisplay(const char* name, const char* mPath, PathNode* parent);
	~AssetDisplay();
	const char* mName;
	const char* mPath;
	std::vector<unsigned int> mUid;
	PathNode* mParent;
	//Resource* mResource;
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

