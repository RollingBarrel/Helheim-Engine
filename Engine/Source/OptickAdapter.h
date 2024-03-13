#pragma once
#include <windows.h>

class OptickAdapter
{
public:
	OptickAdapter() = default;
	virtual ~OptickAdapter();

	void Startup();

	bool IsOpen() const;

private:
	PROCESS_INFORMATION pi = { 0 };
};