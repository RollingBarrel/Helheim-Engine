#ifndef _SCENE_PANEL_H_
#define _SCENE_PANEL_H_
#include "Panel.h"
#define SCENEPANEL "Scene##"

class ScenePanel : public Panel
{
public:
	ScenePanel();
	~ScenePanel();

	void Draw(int windowFlags) override;
private:
	unsigned int prevSizeX = 0;
	unsigned int prevSizeY = 0;
};
#endif /* _SCENE_PANEL_H_ */