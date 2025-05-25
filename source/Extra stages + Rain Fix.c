#include <windows.h>
#include "GG.h"

HMODULE exe;
BYTE* target;
BYTE* patchLocation;
BYTE* StagePointer;

void Stageselectfix() {
	__asm
	{
		cmp eax, 57 // Check if below 57
		jb end; // Skip if
		cmp eax, 59 // Check if above 60
		ja end; // Skip if
		sub eax, 20 // Minus the stage selection pointer by 20 if
		end:
		mov [StagePointer], eax // Set the stage selection pointer in memory (If someone knows how to get it to actually compile as mov [GGXXACPR_Win.exe+710608] please tell me)
	};
}

void Patch() {
	int enable = 1;
	int Paris = 28;
	int Heaven = 39;
	DWORD oldProtect;
	memcpy(target+0x5ed120, &enable, 4);
	memcpy(target+0x5ed144, &enable, 4); // Enable ♯Reload Grave and Heaven in the stage selection
	VirtualProtect(target+0x2196B8, 20, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(target+0x2196B9, &Paris, 1); 
	memcpy(target+0x2196CA, &Heaven, 1); // Correct the stage ID check so that Paris and Heaven's rain works properly
	VirtualProtect(target+0x2196B8, 20, oldProtect, &oldProtect);
	return;
}

void Patch2() {
	DWORD oldProtect;
	char jump[7] = { 0xE8 }; // Write the CALL instruction
	*(DWORD*)(jump + 1) = (DWORD)Stageselectfix - ((DWORD)patchLocation + 5); // Write location for CALL
	VirtualProtect(patchLocation, 7, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(patchLocation, jump, 5);
	VirtualProtect(patchLocation, 7, oldProtect, &oldProtect);

	VirtualProtect((DWORD)Stageselectfix+ 0x11, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy((DWORD)Stageselectfix + 0x11, &StagePointer, 4); //A slightly hacky way to get the asm code in line 18 to actually point to the address in memory aka "mov [GGXXACPR_Win.exe+710608]"
	VirtualProtect((DWORD)Stageselectfix+0x11, 5, oldProtect, &oldProtect);
	return;
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		exe = GetModuleHandle(NULL);
		target = (BYTE*)exe;
		patchLocation = target+0x20659F;
		StagePointer = target+0x710608;
		Patch();
		Patch2();
	}
	return TRUE;
}
