#ifndef _MODULE_RENDER_TEST_H_
#define _MODULE_RENDER_TEST_H_

#include "Module.h"
class ModuleRenderTest :
    public Module
{
public:
	ModuleRenderTest();
	~ModuleRenderTest();

	update_status Update() override;
	
};

#endif /* _MODULE_RENDER_TEST_H_ */
