#ifndef _MODULE_EDITOR_H_
#define _MODULE_EDITOR_H_

#include "Module.h"
#include <map>

struct ImGuiIO;

class Panel;

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;
	bool IsPanelOpen(const char* name) { return mPanels[name]; }

	void ShowMainMenuBar();
	void ResetFloatingPanels(bool openPanels);

private:
	ImGuiIO* io;

	std::map<const char*, Panel*> mPanels;
};

#endif /* _MODULE_EDITOR_H_ */
