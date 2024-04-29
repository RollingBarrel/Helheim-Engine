#ifndef _MODULE_ENGINE_INPUT_H_
#define _MODULE_ENGINE_INPUT_H_

#include "Module.h"
#include "ModuleInput.h"
#include "Globals.h"
#include "KeyboardKeys.h"
#include "float2.h"

class ENGINE_API ModuleEngineInput : public ModuleInput
{
public:

	ModuleEngineInput();
	~ModuleEngineInput();

	update_status PreUpdate(float dt) override;
};

#endif /* _MODULE_ENGINE_INPUT_H_ */