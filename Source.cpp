#pragma comment( lib, "psapi" )
#pragma comment(lib,"detours.lib")													
#pragma comment(lib,"Kernel32.lib")													
#pragma comment(lib,"Winmm.lib")	

#include <common.h>
#include <CWorld.h>
#include <CWanted.h>
#include <CPlayerInfo.h>
#include <CDraw.h>
#include <iostream>
#include "detours.h"													
#include "plugin.h"
#include "Defines.h"
#include "Utils.h"
#include "Memory.h"
#include "MemoryAddresses.h"
#include "GameUtils.h"
#include "GameEvents.h"

enum THREADINFOCLASS
{
	ThreadQuerySetWin32StartAddress = 9,
};

typedef NTSTATUS(__stdcall* f_NtQueryInformationThread)(HANDLE, THREADINFOCLASS, void*, ULONG_PTR, ULONG_PTR*);
typedef unsigned int uint;

using namespace plugin;
using namespace std;

DWORD prevTickCount;
DWORD currentTickCount;
DWORD falseTickCount;

const DWORD multiplier = 2;

extern"C" {
	static DWORD(WINAPI* originalGetTickCount)() = GetTickCount;
}

DWORD WINAPI newGetTickCount() {
	currentTickCount = originalGetTickCount();
	falseTickCount += ((currentTickCount - prevTickCount) * multiplier);
	prevTickCount = currentTickCount;

	return falseTickCount;
}

void GameLoop()
{
	bool flyModeEnabled = false, wallhackEnabled = false, immunityMode = true, aimbotEnabled = true;

	uint lastInterior = *(int*)MemoryAddresses::interior;

	CVector savedPos(0, 0, 0);

	while (1)
	{
		int currentInterior = *(int*)MemoryAddresses::interior;

		if (GetAsyncKeyState(VK_F12) & 1)
		{
			wallhackEnabled ^= true;
			if (wallhackEnabled)
			{
				lastInterior = currentInterior;
				*(int*)MemoryAddresses::interior = 1;
			}
			else
			{
				*(int*)MemoryAddresses::interior = lastInterior;
			}
		}
#ifndef SP
		if (wallhackEnabled)
		{
			*(int*)MemoryAddresses::interior = 1;
		}
		*(int*)fps = 60; //smoother experience
		*(BYTE*)frameLimiter = 0;
#else

#ifdef COMPLETE_JACKASS
		GameUtils::me->SetWantedLevel(0); //who needs a wanted level anyways
#else
		if (GetAsyncKeyState(VK_F4) & 1)
		{
			GameUtils::me->SetWantedLevel(0);
		}
#endif // COMPLETE_JACKASS

		if ((int)GameUtils::me->m_fHealth != 999 || (int)GameUtils::me->m_fArmour != 999)
		{
			GameUtils::me->m_fHealth = 999;
			GameUtils::me->m_fArmour = 999;
		}
#endif // !SP


		if (GetAsyncKeyState(VK_END))
		{
			ExitProcess(0); //quickly exit in case of a crash
		}

		if (GetAsyncKeyState(VK_F4) & 1)
		{
			GameUtils::me->SetJump(); //in case you fall, quickly exit the animation by forcefully jumping
		}

		if (GetAsyncKeyState(VK_F3) & 1)
		{
			GameUtils::Undrunk();
		}

		if (GetAsyncKeyState(VK_F2) & 1)
		{
			constexpr short maxVehicles = 50;
			constexpr float radius = 100.0f; //blow up a maximum of maxVehicles^ in this radius
			short objs = 0;
			CEntity** cars = new CEntity * [maxVehicles];
			CWorld::FindObjectsInRange(GameUtils::me->GetPosition(), radius, true, &objs, maxVehicles, cars, false, true, false, false, false);
			for (short i = 0; i < objs; ++i)
			{
				CVehicle* v = (CVehicle*)cars[i];
				if (v)
				{
					v->BlowUpCar(GameUtils::me);
				}
			}
			if (cars)
				delete[] cars;
		}

		if (GetAsyncKeyState('R'))
		{
			GameUtils::me->m_aWeapons[GameUtils::me->m_nWepSlot].FireSniper(GameUtils::me); //emulate sniper shoot
		}

		if (GetAsyncKeyState(VK_MENU) & GetAsyncKeyState('S')) //save current pos
		{
			if (GameUtils::me->m_bInVehicle)
			{
				savedPos = GameUtils::me->m_pVehicle->GetPosition();
			}
			else
			{
				savedPos = GameUtils::me->GetPosition();
			}
			cout << "CVector(" << savedPos.x << "," << savedPos.y << "," << savedPos.z << ")" << endl;
		}

		if (GetAsyncKeyState(VK_MENU) & GetAsyncKeyState('G')) //go to saved pos
		{
			if (GameUtils::me->m_bInVehicle)
			{
				GameUtils::me->m_pVehicle->Teleport(savedPos);
			}
			else
			{
				GameUtils::me->Teleport(savedPos);
			}
		}

		if (GetAsyncKeyState(VK_MENU) & GetAsyncKeyState(0x31)) //ALT + 1
		{
			CVector downtownAmmu = CVector(-678.996f, 1185.18f, 10.9015f);
			if (GameUtils::me->m_bInVehicle)
			{
				GameUtils::me->m_pVehicle->Teleport(downtownAmmu);
			}
			else
			{
				GameUtils::me->Teleport(downtownAmmu);
			}
		}
		if (GetAsyncKeyState(VK_MENU) & GetAsyncKeyState(0x32)) //ALT + 2
		{
			CVector mansionBack = CVector(-380.241f, -611.161f, 17.2833f);
			if (GameUtils::me->m_bInVehicle)
			{
				GameUtils::me->m_pVehicle->Teleport(mansionBack);
			}
			else
			{
				GameUtils::me->Teleport(mansionBack);
			}
		}
		if (GetAsyncKeyState(VK_MENU) & GetAsyncKeyState(0x33)) //ALT + 3
		{
			CVector antenna1 = CVector(-889.204f, 1111.37f, 92.7426f);
			if (GameUtils::me->m_bInVehicle)
			{
				GameUtils::me->m_pVehicle->Teleport(antenna1);
			}
			else
			{
				GameUtils::me->Teleport(antenna1);
			}
		}
		if (GetAsyncKeyState(VK_MENU) & GetAsyncKeyState(0x34)) //ALT + 4
		{
			CVector antenna2 = CVector(-649.053f, 817.672f, 54.7933f);
			if (GameUtils::me->m_bInVehicle)
			{
				GameUtils::me->m_pVehicle->Teleport(antenna2);
			}
			else
			{
				GameUtils::me->Teleport(antenna2);
			}
		}
		if (GetAsyncKeyState(VK_MENU) & GetAsyncKeyState(0x35)) //ALT + 5
		{
			CVector antenna3 = CVector(-743.002f, 917.253f, 69.4745f);
			if (GameUtils::me->m_bInVehicle)
			{
				GameUtils::me->m_pVehicle->Teleport(antenna3);
			}
			else
			{
				GameUtils::me->Teleport(antenna3);
			}
		}

#ifndef SILENT
#ifdef CONSOLE

		if (GetAsyncKeyState(VK_F7) & 1) //simplest input for the skins that aimbot should avoid 
		{
			GameUtils::avoidedSkins.clear();
			cout << "Avoided skins: ";

			string sSkins;
			getline(cin, sSkins);
			char* a_cSkins = new char[sSkins.size()];
			strcpy(a_cSkins, sSkins.c_str());
			char* p = strtok(a_cSkins, " ");

			while (p)
			{
				GameUtils::avoidedSkins.push_back(stoi(p));
				p = strtok(0, " ");
			}

			delete p;
			delete[] a_cSkins;
			cout << " " << GameUtils::avoidedSkins.size();
			cin.clear();
		}
#endif // CONSOLE

		if (GameUtils::peds.size())
		{
			if (aimbotEnabled)
			{
				if (SniperMode) //SniperMode means you aim continously, which is good for first person view
					//if (SniperMode && ( /*GameUtils::me->m_nWepSlot == GameUtils::me->GetWeaponSlot( WEAPONTYPE_M4 ) || GameUtils::me->m_nWepSlot == GameUtils::me->GetWeaponSlot( WEAPONTYPE_M60 ) ||*/ GameUtils::me->m_nWepSlot == GameUtils::me->GetWeaponSlot(WEAPONTYPE_SNIPERRIFLE) || GameUtils::me->m_nWepSlot == GameUtils::me->GetWeaponSlot(WEAPONTYPE_LASERSCOPE)))
					//only enable if you need auto aimbotEnabled for certain first person weapons
				{
					GameUtils::AimToNearestPed();
				}
				else
				{
					if (KeyPressed(VK_LBUTTON)) //only aim at the press of the LMB
					{
						GameUtils::AimToNearestPed();
					}
				}
			}
		}

#endif // !SILENT


		if (GetAsyncKeyState(VK_F10) & 1)
		{
			flyModeEnabled ^= true;
			if (!flyModeEnabled)
			{
				GameUtils::me->m_nFlags.bUseCollision = true;
			}
		}

		if (GetAsyncKeyState(VK_F6) & 1)
		{
			SniperMode ^= true;
		}

		if (flyModeEnabled)
		{
			GameUtils::FlyThroughStuff(); //fly through everything
		}

		if (GameUtils::me->m_bInVehicle)
		{
			if (GetAsyncKeyState('L'))
			{
				GameUtils::me->m_pVehicle->m_fVelocityFrequency = 0.2f; //faster vehicle while pressing L
			}
			else {
				GameUtils::me->m_pVehicle->m_fVelocityFrequency = 1.0f;
			}
		}

		if (GetAsyncKeyState(VK_F11) & 1)
		{
			immunityMode ^= true;
		}

		if (immunityMode)
		{
			GameUtils::ApplyImmunity(true);
		}
		else
		{
			GameUtils::RemoveImmunity(true);
		}

		Sleep(10);
	}
}

DWORD WINAPI WaitForStuffToInit()
{
	Sleep(20000);
	Memory::nop_((PVOID)(MemoryAddresses::fpsReported), 6);
	return 1;
}

void FindPlayerCharacter()
{
#ifndef SP
	Sleep(17000);

	while (!FindPlayerEntity())
	{
		Sleep(500);
	}

	GameUtils::me = reinterpret_cast<CPlayerPed*>(FindPlayerEntity());
#else
	Sleep(23000);

	while (!FindPlayerPed())
	{
		Sleep(500);
	}

	GameUtils::me = FindPlayerPed();
#endif // !SP
}

void ApplyInitialSettings()
{
#ifndef SP
	prevTickCount = originalGetTickCount();
	falseTickCount = prevTickCount;

	DisableThreadLibraryCalls(Memory::GetCurrentModule());
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)originalGetTickCount, newGetTickCount); //in case you wanna play on multiplayer smoothly on 60 FPS without them noticing, hopefully ;)
	DetourTransactionCommit();
#else
	CWorld::Players[0].m_bNeverGetsTired = true;

	CWorld::Players[0].m_nMoney = 99999999;

	CWorld::Players[0].m_nMaxHealth = (unsigned char)999;
	CWorld::Players[0].m_nMaxArmour = (unsigned char)999;
#endif // !SP
}

DWORD WINAPI Init()
{
#ifdef CONSOLE
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif // CONSOLE

	FindPlayerCharacter();
	ApplyInitialSettings();

	MemoryAddresses::CalculateMemoryAddresses();
	MemoryAddresses::ApplyInfiniteAmmoCheat();

	Events::pedCtorEvent.Add(OnPedCreated);
	Events::pedDtorEvent.Add(OnPedDestroyed);
	Events::vehicleCtorEvent.Add(OnVehicleCreated);
	Events::vehicleDtorEvent.Add(OnVehicleDestroyed);

	CWorld::bIncludeDeadPeds = true;

	GameLoop();
	return 0;
}

class betterVC
{
public:
	betterVC()
	{
		DWORD thread_id;

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Init, 0, 0, &thread_id);
#ifndef SP
		DWORD thread_id2;
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)WaitForStuffToInit, 0, 0, &thread_id2);
#endif // !SP

	}
} _betterVC;