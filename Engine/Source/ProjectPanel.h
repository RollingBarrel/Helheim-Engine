#pragma once
#include "Panel.h"
#define PROJECTPANEL "Project##"

#include <vector>

struct PathNode;
class Resource;
struct AssetDisplay;

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

	std::string mSearchAsset = "";

	PathNode* mSelectedNode = nullptr;
	AssetDisplay* mSelectedAsset = nullptr;
};

