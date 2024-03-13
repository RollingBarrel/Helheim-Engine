#pragma once
#include "Panel.h"

#define TAGSMANAGERPANEL "TagsManagerPanel##"

class Tag;

class TagsManagerPanel: public Panel
{
public:
	TagsManagerPanel();
	~TagsManagerPanel();

private:
	void Draw(int windowFlags) override;
	void DrawTagsList();
	void DrawDeleteButton();
	void DrawAddButton();

	int mSelectedIdx = -1;
	Tag* mSelectedTag = nullptr;
};

