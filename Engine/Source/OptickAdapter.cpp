#include "OptickAdapter.h"
#include "Globals.h"

OptickAdapter::~OptickAdapter() {
	//Close Optick when Terminating Engine
	TerminateProcess(pi.hProcess, 1);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void OptickAdapter::Startup() {
	if (IsOpen()) { return; }

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess("Optick.exe",   // No module name (use command line)
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		LOG("CreateProcess failed (%d).\n", GetLastError());
		return;
	}

}

bool OptickAdapter::IsOpen() const {
	DWORD exitCode;
	if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
		return false;
	}
	return exitCode == STILL_ACTIVE;
}