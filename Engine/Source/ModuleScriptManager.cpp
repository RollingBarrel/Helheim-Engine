#define _CRT_SECURE_NO_WARNINGS

#include "ModuleScriptManager.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "NavMeshController.h"
#include "ModuleResource.h"
#include "Script.h"
#include "ScriptComponent.h"
#include "GameObject.h"
#include <Windows.h>
#include <string>
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include <any>

static bool PDBReplace(const std::string& filename, const std::string& namePDB);

ModuleScriptManager::ModuleScriptManager()
{
}

ModuleScriptManager::~ModuleScriptManager()
{
}

bool ModuleScriptManager::Init()
{
#ifdef ENGINE
	CopyFile("../Scripting/Output/Scripting.dll", "Scripting.dll", false);
	mHandle = LoadLibrary("Scripting.dll");
	int b = GetLastError();
	mLastModificationTime = App->GetFileSystem()->GetLastModTime("Scripting.dll");

	UpdateScripts();
#else
	CopyFile("../Scripting/Output/Scripting.dll", "Scripting.dll", false);
	mHandle = LoadLibrary("Scripting.dll");
	int b = GetLastError();
#endif // ENGINE
	return true;
}

update_status ModuleScriptManager::PreUpdate(float dt)
{
#ifdef ENGINE
	UpdateScripts();

	int64_t modificationTime = App->GetFileSystem()->GetLastModTime("Scripting.dll");
	if (mLastModificationTime != modificationTime && !mIsPlaying)
	{
		mLastModificationTime = modificationTime;
		HotReload();
	}

#endif // ENGINE
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScriptManager::Update(float dt)
{
	if (mIsPlaying) 
	{
		for (unsigned int i = 0; i < mScripts.size(); ++i) 
		{
			mScripts[i]->mScript->Update();
		}
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScriptManager::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

bool ModuleScriptManager::CleanUp()
{
	FreeLibrary(static_cast<HMODULE>(mHandle));
	return true;
}

void ModuleScriptManager::AddScript(ScriptComponent* script) 
{ 
	mScripts.push_back(script); 
}

void ModuleScriptManager::RemoveScript(ScriptComponent* script)
{
	std::vector<ScriptComponent*>::iterator deletePos = mScripts.end();
	for (std::vector<ScriptComponent*>::iterator it = mScripts.begin(); it != mScripts.end(); ++it) 
	{

		if (*it == script) 
		{
			deletePos = it;
			break;
		}
		
	}
	if (deletePos != mScripts.end()) 
	{
		mScripts.erase(deletePos);
	}
		
}

void ModuleScriptManager::HotReload()
{
	std::vector<std::vector<std::pair<Member, void*>>> oldScripts;
	SaveOldScript(oldScripts);

	FreeLibrary(static_cast<HMODULE>(mHandle));
	while (CopyFile("../Scripting/Output/Scripting.dll", "Scripting.dll", false) == FALSE) {}
	//while (CopyFile("../Scripting/Output/Scripting.pdb", "./Scripting.pdb", false) == FALSE) {}
	//bool replaced = PDBReplace("./Scripting.dll", "./Scripting.pdb");
	mHandle = LoadLibrary("Scripting.dll");
	ReloadScripts(oldScripts);
	LOG("HOTRELOADING COMPLETE");
}

void ModuleScriptManager::ReloadScripts(const std::vector<std::vector<std::pair<Member, void*>>>& oldScripts)
{
	for (unsigned int i = 0; i < mScripts.size(); ++i) 
	{

		std::vector<std::pair<Member, void*>> oldScript = oldScripts[i];


		
		mScripts[i]->LoadScript(mScripts[i]->GetScriptName());

		Script* newScript = mScripts[i]->mScript;

		const std::vector<Member*> newMembers = newScript->GetMembers();
		
		
		for (unsigned int j = 0; j < oldScript.size(); ++j) 
		{

			for (unsigned int k = 0; k < newMembers.size(); ++k) 
			{
				
				if (strcmp(oldScript[j].first.mName, newMembers[k]->mName) == 0) {
					char* newScriptPos = ((char*)newScript) + newMembers[k]->mOffset;
					switch (newMembers[k]->mType)
					{
					case(MemberType::INT):
						*reinterpret_cast<int*>(newScriptPos) = *reinterpret_cast<int*>(oldScript[j].second);
						delete reinterpret_cast<int*>(oldScript[j].second);
						break;
					case(MemberType::FLOAT):
						*reinterpret_cast<float*>(newScriptPos) = *reinterpret_cast<float*>(oldScript[j].second);
						delete reinterpret_cast<float*>(oldScript[j].second);
						break;
					case(MemberType::BOOL):
						*reinterpret_cast<bool*>(newScriptPos) = *reinterpret_cast<bool*>(oldScript[j].second);
						delete reinterpret_cast<bool*>(oldScript[j].second);
						break;
					case(MemberType::GAMEOBJECT):
						*reinterpret_cast<GameObject**>(newScriptPos) = reinterpret_cast<GameObject*>(oldScript[j].second);
						break;
					case(MemberType::FLOAT3):
						*reinterpret_cast<float3*>(newScriptPos) = *reinterpret_cast<float3*>(oldScript[j].second);
						delete reinterpret_cast<float3*>(oldScript[j].second);
						break;
					}
				}

			}

		}

	}

}

void ModuleScriptManager::SaveOldScript(std::vector<std::vector<std::pair<Member, void*>>>& oldScripts)
{
	for (ScriptComponent* scriptComponent : mScripts)
	{

		Script* oldScript = scriptComponent->mScript;

		const std::vector<Member*> oldMembersPointer = oldScript->GetMembers();
		std::vector<std::pair<Member, void*>> oldMembers;

		for (Member* member : oldMembersPointer)
		{
			oldMembers.push_back(std::pair<Member, void*>(*member, nullptr));
			oldMembers.back().first.mName = new char[strlen(member->mName)];
			strcpy(const_cast<char*>(oldMembers.back().first.mName), member->mName);

			char* oldScriptPos = ((char*)oldScript) + oldMembers.back().first.mOffset;

			switch (member->mType)
			{
			case(MemberType::INT):
				oldMembers.back().second = new int(*reinterpret_cast<int*>(oldScriptPos));
				break;
			case(MemberType::FLOAT):
				oldMembers.back().second = new float(*reinterpret_cast<float*>(oldScriptPos));
				break;
			case(MemberType::BOOL):
				oldMembers.back().second = new bool(*reinterpret_cast<bool*>(oldScriptPos));
				break;
			case(MemberType::GAMEOBJECT):
				oldMembers.back().second = *reinterpret_cast<GameObject**>(oldScriptPos);
				break;
			case(MemberType::FLOAT3):
				oldMembers.back().second = new float3(*reinterpret_cast<float3*>(oldScriptPos));
				break;
			default:
				break;
			}

		}


		oldScripts.push_back(oldMembers);
	}
}

void ModuleScriptManager::Play()
{
	mIsPlaying = true;
}

void ModuleScriptManager::Stop()
{
	mIsPlaying = false;
	App->GetAudio()->PauseAllChannels();
}

void ModuleScriptManager::Start()
{

	if (App->GetScene()->GetRoot()->GetName() == "Level1")
	{
		App->GetScene()->GetNavController()->HandleBuild();
	}

	mIsPlaying = true;
	for (ScriptComponent* script : mScripts) 
	{
		script->mScript->Start();
	}
}



//DO NOT ERASE THESE FUNCTIONS UNTIL WE ARE SURE WE DOES NOT NEED IT AFTER ENGINE/GAME SPLIT


template<class T>
static T struct_cast(void* ptr, LONG offset = 0) {
	return reinterpret_cast<T>(reinterpret_cast<intptr_t>(ptr) + offset);
}

struct RSDSHeader {
	DWORD signature;
	GUID guid;
	long version;
	char filename[1];
};


// Defer -----------
// Defer calls the given code at the end of the scope. Useful for freeing resources.
// Important: End of scope is not end of function. Be careful when using it inside loops.
// Usage: DEFER{ statements; };
#ifndef DEFER
struct defer_dummy {};

template<class F>
struct deferrer {
	F f;
	~deferrer() {
		f();
	}
};

template<class F>
deferrer<F> operator*(defer_dummy, F f) {
	return { f };
}

#define DEFER__(LINE) zz_defer##LINE
#define DEFER_(LINE) DEFER__(LINE)
#define DEFER auto DEFER_(__LINE__) = defer_dummy {}* [&]()
#endif // DEFER

static bool DebugDirRelativeVirtualAddress(PIMAGE_OPTIONAL_HEADER optionalHeader, DWORD& debugDirRva, DWORD& debugDirSize) {
	if (optionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) 
	{
		// Represents the optional header format.
		PIMAGE_OPTIONAL_HEADER64 optionalHeader64 = struct_cast<PIMAGE_OPTIONAL_HEADER64>(optionalHeader);
		debugDirRva = optionalHeader64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
		debugDirSize = optionalHeader64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
	}
	else 
	{
		// Represents the optional header format.
		PIMAGE_OPTIONAL_HEADER32 optionalHeader32 = struct_cast<PIMAGE_OPTIONAL_HEADER32>(optionalHeader);
		debugDirRva = optionalHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
		debugDirSize = optionalHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
	}

	if (debugDirRva == 0 && debugDirSize == 0) 
	{
		return true;
	}
	else if (debugDirRva == 0 || debugDirSize == 0) 
	{
		return false;
	}

	return true;
}

static bool FileOffsetRelativeVirtualAdress(PIMAGE_NT_HEADERS ntHeaders, DWORD rva, DWORD& fileOffset) {
	bool found = false;

	// Represents the image section header format.
	IMAGE_SECTION_HEADER* sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
	for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections && !found; ++i, ++sectionHeader) 
	{
		if (sectionHeader->VirtualAddress <= rva) 
		{
			if (sectionHeader->VirtualAddress + sectionHeader->Misc.VirtualSize > rva) 
			{
				found = true;
				fileOffset = rva - sectionHeader->VirtualAddress + sectionHeader->PointerToRawData;
			}
		}
	}

	if (!found) 
	{
		return false;
	}

	return true;
}


static char* PDBFind(LPBYTE imageBase, PIMAGE_DEBUG_DIRECTORY debugDir) {
	assert(debugDir && imageBase);
	LPBYTE debugInfo = const_cast<LPBYTE>(imageBase + debugDir->PointerToRawData);
	const DWORD debugInfoSize = debugDir->SizeOfData;

	if (debugInfo == 0 || debugInfoSize == 0) 
	{
		return nullptr;
	}

	if (IsBadReadPtr(debugInfo, debugInfoSize)) 
	{
		return nullptr;
	}

	if (debugInfoSize < sizeof(DWORD)) 
	{
		return nullptr;
	}

	if (debugDir->Type == IMAGE_DEBUG_TYPE_CODEVIEW) 
	{
		auto signature = *(DWORD*)debugInfo;
		if (signature == 'SDSR') 
		{
			auto* info = (RSDSHeader*)(debugInfo);
			if (IsBadReadPtr(debugInfo, sizeof(RSDSHeader))) 
			{
				return nullptr;
			}
			if (IsBadStringPtrA((const char*)info->filename, UINT_MAX)) 
			{
				return nullptr;
			}
			return info->filename;
		}
	}

	return nullptr;
}

static bool PDBReplace(const std::string& filename, const std::string& namePDB) {
	HANDLE fp = nullptr;
	HANDLE filemap = nullptr;
	LPVOID mem = nullptr;
	bool result = false;

	DEFER
	{
		if (mem != nullptr) 
		{
			UnmapViewOfFile(mem);
		}

		if (filemap != nullptr) 
		{
			CloseHandle(filemap);
		}

		if ((fp != nullptr) && (fp != INVALID_HANDLE_VALUE)) 
		{
			CloseHandle(fp);
		}
	};

	fp = CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if ((fp == INVALID_HANDLE_VALUE) || (fp == nullptr)) 
	{
		return false;
	}

	// Creates or opens a named or unnamed file mapping object for a specified file.
	filemap = CreateFileMapping(fp, nullptr, PAGE_READWRITE, 0, 0, nullptr);
	if (filemap == nullptr) 
	{
		return false;
	}

	// Maps a view of a file mapping into the address space of a calling process.
	mem = MapViewOfFile(filemap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (mem == nullptr) 
	{
		return false;
	}

	//  Represents the DOS header format.
	PIMAGE_DOS_HEADER dosHeader = struct_cast<PIMAGE_DOS_HEADER>(mem);
	if (dosHeader == 0) 
	{
		return false;
	}

	// Verifies that the calling process has read access to the specified range of memory.
	if (IsBadReadPtr(dosHeader, sizeof(IMAGE_DOS_HEADER))) 
	{
		return false;
	}

	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) 
	{
		return false;
	}

	// Represents the PE header format.
	PIMAGE_NT_HEADERS ntHeaders = struct_cast<PIMAGE_NT_HEADERS>(dosHeader, dosHeader->e_lfanew);
	if (ntHeaders == 0) 
	{
		return false;
	}

	// Verifies that the calling process has read access to the specified range of memory.
	if (IsBadReadPtr(ntHeaders, sizeof(ntHeaders->Signature))) 
	{
		return false;
	}

	// Check if signature of the NT header is the correct one
	if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) 
	{
		return false;
	}

	// Verifies that the calling process has read access to the specified range of memory.
	if (IsBadReadPtr(&ntHeaders->FileHeader, sizeof(IMAGE_FILE_HEADER))) 
	{
		return false;
	}

	if (IsBadReadPtr(&ntHeaders->OptionalHeader, ntHeaders->FileHeader.SizeOfOptionalHeader)) 
	{
		return false;
	}

	if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC && ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) 
	{
		return false;
	}

	// Retrieve the contents of PE file sections
	auto sectionHeaders = IMAGE_FIRST_SECTION(ntHeaders);
	if (IsBadReadPtr(sectionHeaders, ntHeaders->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER))) 
	{
		return false;
	}

	DWORD debugDirVRA = 0;
	DWORD debugDirSize = 0;
	if (!DebugDirRelativeVirtualAddress(&ntHeaders->OptionalHeader, debugDirVRA, debugDirSize)) 
	{
		return false;
	}

	if (debugDirVRA == 0 || debugDirSize == 0) 
	{
		return false;
	}

	DWORD offset = 0;
	if (!FileOffsetRelativeVirtualAdress(ntHeaders, debugDirVRA, offset)) 
	{
		return false;
	}

	// Represents the debug directory format.
	PIMAGE_DEBUG_DIRECTORY debugDir = struct_cast<PIMAGE_DEBUG_DIRECTORY>(mem, offset);
	if (debugDir == 0) 
	{
		return false;
	}

	if (IsBadReadPtr(debugDir, debugDirSize)) 
	{
		return false;
	}

	if (debugDirSize < sizeof(IMAGE_DEBUG_DIRECTORY)) 
	{
		return false;
	}

	int numEntries = debugDirSize / sizeof(IMAGE_DEBUG_DIRECTORY);
	if (numEntries == 0) 
	{
		return false;
	}

	for (int i = 1; i <= numEntries; ++i, ++debugDir) 
	{
		char* pdb = PDBFind((LPBYTE)mem, debugDir);
		if (pdb) 
		{
			size_t len = strlen(pdb);
			if (len >= strlen(namePDB.c_str())) 
			{
				memcpy_s(pdb, len, namePDB.c_str(), namePDB.length());
				pdb[namePDB.length()] = 0;
				result = true;
			}
		}
	}

	return result;
}

void ModuleScriptManager::UpdateScripts()
{
#ifdef ENGINE
	std::vector<std::string> files;
	App->GetFileSystem()->GetDirectoryFiles("Assets/Scripts", files);

	for (std::string file : files)
	{
		std::string filePath = "Assets/Scripts/";
		filePath += file;

		if (!App->GetFileSystem()->IsDirectory(filePath.c_str()))
		{
			if (filePath.back() == 'h')
			{
				std::string filePathWithMeta;
				filePathWithMeta = filePath + ".emeta";
				if (!App->GetFileSystem()->Exists(filePathWithMeta.c_str()))
				{
					filePath = "./" + filePath;
					App->GetResource()->ImportFile(filePath.c_str());
				}
			}
			else if (filePath.find(".emeta") != std::string::npos)
			{
				int pos = filePath.find(".emeta");
				filePath.erase(pos);
				if (!App->GetFileSystem()->Exists(filePath.c_str()))
				{
					App->GetFileSystem()->RemoveFile(filePath.c_str());
				}
			}
		}
	}
#endif
}