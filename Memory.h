#pragma once
#include <Windows.h>

#define NOP 0x90

namespace Memory
{
	void nop_(PVOID address, int bytes) {
		DWORD d, ds;
		VirtualProtect(address, bytes, PAGE_EXECUTE_READWRITE, &d);
		memset(address, NOP, bytes); VirtualProtect(address, bytes, d, &ds);
	}

	const HMODULE GetCurrentModule()
	{
		DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS;
		HMODULE hm = 0;
		::GetModuleHandleEx(flags, reinterpret_cast<LPCTSTR>(GetCurrentModule), &hm);
		return hm;
	}
}
