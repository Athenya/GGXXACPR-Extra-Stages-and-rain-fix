#include <windows.h>
#include "GG.h"

HMODULE exe;
BYTE* target;
BYTE* patchLocation;
BYTE* StagePointer;


void Patch() {
	int Paris = 28;
	int Heaven = 39;
	DWORD oldProtect;
	VirtualProtect(target+0x2196B8, 20, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(target+0x2196B9, &Paris, 1); 
	memcpy(target+0x2196CA, &Heaven, 1); // Correct the stage ID check so that Paris and Heaven's rain works properly
	VirtualProtect(target+0x2196B8, 20, oldProtect, &oldProtect);
	return;
}


BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		exe = GetModuleHandle(NULL);
		target = (BYTE*)exe;
		patchLocation = target+0x20659F;
		StagePointer = target+0x710608;
		Patch();
	}
	return TRUE;
}
