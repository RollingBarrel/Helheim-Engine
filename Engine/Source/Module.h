#ifndef _MODULE_H_
#define _MODULE_H_

#include "Globals.h"
#include "HelheimEvent.h"

class Module
{
public:

	Module()
	{
	}

    virtual ~Module()
    {
    }

	virtual bool Init() 
	{
		return true; 
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void ReceiveEvent(HelheimEvent& e)
	{
	}
};

#endif /* _MODULE_H_ */