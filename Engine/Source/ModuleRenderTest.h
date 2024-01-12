#ifndef _MODULE_RENDER_TEST_H_
#define _MODULE_RENDER_TEST_H_

#include "Module.h"
class ModuleRenderTest :
    public Module
{
public:
	ModuleRenderTest();
	~ModuleRenderTest();

	bool Init()override;
	update_status Update() override;

private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int programId;
	unsigned int textureId;
};

#endif /* _MODULE_RENDER_TEST_H_ */
