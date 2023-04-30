#pragma once

constexpr const char* gameModuleName = "vcmp-game.dll";

constexpr uint xAxisOffset = 0x3725EC;
constexpr uint yAxisOffset = 0x3725DC;
constexpr uint fpsHookOffset = 0x15ED0B;
constexpr uint staticInterior = 0x978810;
constexpr uint interiorOffset = 0x369464;
constexpr uint fps = 0x09b48ec;
constexpr uint frameLimiter = 0x869655;
constexpr uint drawDist = 0x00690220;
constexpr uint gameCompletion = 0x009B6CDC;
constexpr uint someOtherStuff = 0x00974B0C;
constexpr uint someAddressWhichNeedsToBeNopped = 0x004CE46A;
constexpr uint stuntBikeOffset = 0x0036F3A0;
constexpr uint stuntBikePointerOffset = 0x538;

namespace MemoryAddresses
{
	uint x_axis, y_axis;
	uint fpsReported;
	uint interior;

	DWORD temp;
	unsigned long temp2;

	void ApplyInfiniteAmmoCheat()
	{
		*(float*)gameCompletion = 100.0f;
		*(float*)someOtherStuff = 100.0f;
		Memory::nop_((PVOID)(someAddressWhichNeedsToBeNopped), 10);
	}

	void CalculateMemoryAddresses()
	{
#ifndef SP
		x_axis = (DWORD)GetModuleHandle(gameModuleName) + xAxisOffset;
		y_axis = (DWORD)GetModuleHandle(gameModuleName) + yAxisOffset;
		fpsReported = (DWORD)GetModuleHandle(gameModuleName) + fpsHookOffset;
		interior = (DWORD)GetModuleHandle(gameModuleName) + interiorOffset;
#else
		interior = staticInterior;
		x_axis = 0x7E48CC;
		y_axis = 0x7E48BC;
#endif // !SP

	}
}