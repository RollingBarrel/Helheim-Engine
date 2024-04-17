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
	void SavePrefab(const PathNode& dir) const;

private:
	const void DrawFolders(const PathNode& current);
	const void DrawAssets(const PathNode& current);

	PathNode* mSelectedNode = nullptr;
	AssetDisplay* mSelectedAsset = nullptr;
};

