#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../util/patch.h"
#include "../chatwindow.h"
#include "../game/actionstuff.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow* pChatWindow;

PAD_KEYS LocalPlayerKeys;
PAD_KEYS RemotePlayerKeys[PLAYER_PED_SLOTS];

uintptr_t dwCurPlayerActor = 0;
uintptr_t dwCurWeaponProcessingPlayer = 0;
uint8_t byteCurPlayer = 0;
uint8_t byteCurDriver = 0;
uint8_t byteInternalPlayer = 0;
uint16_t wSavedCameraMode2 = 0;
uint8_t byteSavedCameraMode;

uint16_t (*CPad__GetPedWalkLeftRight)(uintptr_t thiz);
uint16_t CPad__GetPedWalkLeftRight_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyLR;
		if((dwResult == 0xFF80 || dwResult == 0x80) &&  
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK])
		{
			dwResult = 0x40;
		}
		return dwResult;
	}
	else
	{
		// Local player
		LocalPlayerKeys.wKeyLR = CPad__GetPedWalkLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t (*CPad__GetPedWalkUpDown)(uintptr_t thiz);
uint16_t CPad__GetPedWalkUpDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyUD;
		if((dwResult == 0xFF80 || dwResult == 0x80) &&  
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK])
		{
			dwResult = 0x40;
		}
		return dwResult;
	}
	else
	{
		// Local player
		LocalPlayerKeys.wKeyUD = CPad__GetPedWalkUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint32_t (*CPad__GetSprint)(uintptr_t thiz, uint32_t unk);
uint32_t CPad__GetSprint_hook(uintptr_t thiz, uint32_t unk)
{	
	/*if (pNetGame)
	{
		PED_TYPE* localPed = util::game::pool::FindPlayerPed();
		if(localPed)
		{	
			PLAYERID id = pNetGame->GetPlayerPool()->FindRemotePlayerIDFromGtaPtr(localPed);
			if(id != INVALID_PLAYER_ID) if(pNetGame->GetPlayerPool()->GetAt(id)->GetPlayerPed()->IsInVehicle()) return 0;
		}
	}*/
	
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT];
	}
	else
	{	
		if(pNetGame)
		{	
			if (!pGame->FindPlayerPed()->IsInVehicle())
			{
				LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = CPad__GetSprint(thiz, unk);
				return LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
			}
			else return 0;
		}
		else CPad__GetSprint(thiz, unk);
	}

	return CPad__GetSprint(thiz, unk);
}

uint32_t (*CPad__JumpJustDown)(uintptr_t thiz);
uint32_t CPad__JumpJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(!RemotePlayerKeys[byteCurPlayer].bIgnoreJump &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			!RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
		{
			RemotePlayerKeys[byteCurPlayer].bIgnoreJump = true;
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
		}

		return 0;
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetJump)(uintptr_t thiz);
uint32_t CPad__GetJump_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(RemotePlayerKeys[byteCurPlayer].bIgnoreJump) return 0;
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAutoClimb)(uintptr_t thiz);
uint32_t CPad__GetAutoClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAbortClimb)(uintptr_t thiz);
uint32_t CPad__GetAbortClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
}

uint32_t (*CPad__DiveJustDown)();
uint32_t CPad__DiveJustDown_hook()
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__DiveJustDown();
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uint32_t (*CPad__SwimJumpJustDown)(uintptr_t thiz);
uint32_t CPad__SwimJumpJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__SwimJumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__DuckJustDown)(uintptr_t thiz, int unk);
uint32_t CPad__DuckJustDown_hook(uintptr_t thiz, int unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(IsRemotePlayerDucking(byteCurPlayer) && IsRemotePlayerDuckingReset(byteCurPlayer))
		{
			SetRemotePlayerDucking(byteCurPlayer, 0);
			SetRemotePlayerDuckingReset(byteCurPlayer, 0);
			return 1;
		}
		return 0;
	}
	else
	{
		return CPad__DuckJustDown(thiz, unk);
	}
}

uint32_t (*CPad__MeleeAttackJustDown)(uintptr_t thiz);
uint32_t CPad__MeleeAttackJustDown_hook(uintptr_t thiz)
{
	/*
		0 - �� ����
		1 - ������� ���� (���)
		2 - ������� ���� (��� + F)
	*/

	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK])
			return 2;

		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint32_t dwResult = CPad__MeleeAttackJustDown(thiz);
		//LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = true;

		//if(dwResult == 2) 
		//{
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
		//}
		//else if(dwResult == 1)
		//{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = dwResult;
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = false;
		//}

		return dwResult;
	}
}

uint32_t (*CPad__GetBlock)(uintptr_t thiz);
uint32_t CPad__GetBlock_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
			return 1;

		return 0;
	}
	else
	{
		return CPad__GetBlock(thiz);
	}
}

int16_t (*CPad__GetSteeringLeftRight)(uintptr_t thiz);
int16_t CPad__GetSteeringLeftRight_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return (int16_t)RemotePlayerKeys[byteCurDriver].wKeyLR;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyLR = CPad__GetSteeringLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t (*CPad__GetSteeringUpDown)(uintptr_t thiz);
uint16_t CPad__GetSteeringUpDown_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].wKeyUD;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyUD = CPad__GetSteeringUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint16_t (*CPad__GetAccelerate)(uintptr_t thiz);
uint16_t CPad__GetAccelerate_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_SPRINT] ? 0xFF : 0x00;
	}
	else
	{	
		if(pGame)
		{
			if (pGame->FindPlayerPed()->IsInVehicle())
			{
				// local player
				uint16_t wAccelerate = CPad__GetAccelerate(thiz);
				LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = wAccelerate;
				return wAccelerate;
			}
			else return 0;
		}
	}
}

uint16_t (*CPad__GetBrake)(uintptr_t thiz);
uint16_t CPad__GetBrake_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_JUMP] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint16_t wBrake = CPad__GetBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = wBrake;
		return wBrake;
	}
}

uint32_t (*CPad__GetHandBrake)(uintptr_t thiz);
uint32_t CPad__GetHandBrake_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_HANDBRAKE] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint32_t handBrake = CPad__GetHandBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = handBrake;
		return handBrake;
	}
}

uint32_t (*CPad__GetHorn)(uintptr_t thiz);
uint32_t CPad__GetHorn_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_CROUCH];
	}
	else
	{
		// local player
		uint32_t horn = CPad__GetHorn(thiz);
		//Log("horn: %d", horn);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad__GetHorn(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

uint32_t (*CPad__ExitVehicleJustDown)(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec);
uint32_t CPad__ExitVehicleJustDown_hook(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec)
{
	CPlayerPool *pPlayerPool;
	CLocalPlayer *pLocalPlayer;

	if(pNetGame)
	{
		pPlayerPool = pNetGame->GetPlayerPool();
		if(pPlayerPool)
		{
			pLocalPlayer = pPlayerPool->GetLocalPlayer();
			if(pLocalPlayer)
			{
				if( pLocalPlayer->HandlePassengerEntry() )
					return 0;
			}
		}
	}

	return CPad__ExitVehicleJustDown(thiz, a2, vehicle, a4, vec);
}

void (*CPed__ProcessControl)(uintptr_t thiz);
void CPed__ProcessControl_hook(uintptr_t thiz)
{
	dwCurPlayerActor = thiz;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);
	
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// REMOTE PLAYER
		
		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);
		
		// save the second internal cammode, apply the context
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if(*wCameraMode2 == 4) *wCameraMode2 = 0;

		// save the camera zoom factor, apply the context
		GameStoreLocalPlayerCameraExtZoomAndAspect();
		GameSetRemotePlayerCameraExtZoomAndAspect(byteCurPlayer);
		
		// aim & Skills switching
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);
		GameStoreLocalPlayerSkills();
		GameSetRemotePlayerSkills(byteCurPlayer);
		
		*pbyteCurrentPlayer = byteCurPlayer;

		// CPed::UpdatePosition nulled from CPed::ProcessControl
		CPatch::NOP(g_libGTASA+0x439B7A, 2);

		// call original
		CPed__ProcessControl(thiz);
		
		// restore
		CPatch::WriteMemory(g_libGTASA+0x439B7A, "\xFA\xF7\x1D\xF8", 4);
		
		GameSetLocalPlayerSkills();
		
		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;
		
		// remote the local player's camera zoom factor
		GameSetLocalPlayerCameraExtZoomAndAspect();
		
		*pbyteCurrentPlayer = 0;
		
		// restore the local player's keys and the internal ID.
		GameSetLocalPlayerAim();
	}
	else
	{
		// LOCAL PLAYER

		// Apply the original code to set ped rot from Cam
		CPatch::WriteMemory(g_libGTASA+0x4BED92, "\x10\x60", 2);

		(*CPed__ProcessControl)(thiz);

		// Reapply the no ped rots from Cam patch
		CPatch::WriteMemory(g_libGTASA+0x4BED92, "\x00\x46", 2);
	}

	return;
}

void AllVehicles__ProcessControl_hook(uintptr_t thiz)
{
	VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE*)thiz;
	uintptr_t this_vtable = pVehicle->entity.vtable;
	this_vtable -= g_libGTASA;

	uintptr_t call_addr = 0;

	switch(this_vtable)
	{
		// CAutomobile
		case 0x5CC9F0:
		call_addr = 0x4E314C;
		break;

		// CBoat
		case 0x5CCD48:
		call_addr = 0x4F7408;
		break;

		// CBike
		case 0x5CCB18:
		call_addr = 0x4EE790;
		break;

		// CPlane
		case 0x5CD0B0:
		call_addr = 0x5031E8;
		break;

		// CHeli
		case 0x5CCE60:
		call_addr = 0x4FE62C;
		break;

		// CBmx
		case 0x5CCC30:
		call_addr = 0x4F3CE8;
		break;

		// CMonsterTruck
		case 0x5CCF88:
		call_addr = 0x500A34;
		break;

		// CQuadBike
		case 0x5CD1D8:
		call_addr = 0x505840;
		break;

		// CTrain
		case 0x5CD428:
		call_addr = 0x50AB24;
		break;
	}

	if(pVehicle && pVehicle->pDriver)
	{
		byteCurDriver = FindPlayerNumFromPedPtr((uintptr_t)pVehicle->pDriver);
	}

	if(pVehicle->pDriver && pVehicle->pDriver->dwPedType == 0 &&
		pVehicle->pDriver != GamePool_FindPlayerPed() && 
		*(uint8_t*)(g_libGTASA + 0x8E864C) == 0) // CWorld::PlayerInFocus
	{
		*(uint8_t*)(g_libGTASA + 0x8E864C) = 0;

		pVehicle->pDriver->dwPedType = 4;
		//CAEVehicleAudioEntity::Service
		(( void (*)(uintptr_t))(g_libGTASA + 0x364B64+1))(thiz+0x138);
		pVehicle->pDriver->dwPedType = 0;
	}
	else
	{
		(( void (*)(uintptr_t))(g_libGTASA + 0x364B64+1))(thiz+0x138);
	}

	// VEHTYPE::ProcessControl()
    (( void (*)(VEHICLE_TYPE*))(g_libGTASA + call_addr+1))(pVehicle);
}

uint32_t ExecuteOriginalTaskUseGun(uintptr_t thiz, uintptr_t a2)
{
	uintptr_t v2;
	uintptr_t v3;
	int v4;
	int v5;
	int v6;
	int result;
	char v8;
	
	v2 = thiz;
	v3 = a2;
	v4 = *((uintptr_t*)thiz + 12);
	v5 = *((uintptr_t*)a2 + 7 * *((unsigned char*)a2 + 1816) + 360);
	v6 = reinterpret_cast<int(*)(uintptr_t)>(g_libGTASA, 0x434F24 + 1)(a2);
	int weapon_info = reinterpret_cast<int(*)(int, int)>(g_libGTASA, 0x56BD60 + 1)(v5, v6);
	if(v4 == weapon_info)
	{
		v8 = *((unsigned char*)v2 + 13);
		if(*((unsigned char*)v2 + 13) & 1)
		{
			reinterpret_cast<int(*)(uintptr_t, uintptr_t, int)>(g_libGTASA, 0x46D4E4 + 1)(v2,v3,0);
			v8 = *((unsigned char*)v2 + 13);
		}
		if(v8 & 2)
		{
			reinterpret_cast<int(*)(uintptr_t, uintptr_t, int)>(g_libGTASA, 0x46D4E4 + 1)(v2,v3,1);
		}
		
		result = 0;
		*((unsigned char*)v2 + 13) = 0;
	}
	else
	{
		result = 0;
	}
	return result;
}

void CTaskSimpleUseGun__SetPedPosition_hook(uintptr_t thiz, PED_TYPE* pPed)
{
	//Log("CTaskSimpleUseGun__SetPedPosition");
	dwCurPlayerActor = (uintptr_t)pPed;
	byteCurPlayer = FindPlayerNumFromPedPtr((uintptr_t)pPed);
	byteInternalPlayer = *pbyteCurrentPlayer;

	if (dwCurPlayerActor && (byteCurPlayer != 0) && 
		byteInternalPlayer == 0) // not local player and local player's keys set.
	{
		// remote player
		
		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);
		
		// save the second internal cammode, apply the context
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if(*wCameraMode2 == 4) *wCameraMode2 = 0;
		
		// save the camera zoom factor, apply the context
		GameStoreLocalPlayerCameraExtZoomAndAspect();
		GameSetRemotePlayerCameraExtZoomAndAspect(byteCurPlayer);
		
		// aim switching
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);
		GameStoreLocalPlayerSkills();
		GameSetRemotePlayerSkills(byteCurPlayer);
		
		*pbyteCurrentPlayer = byteCurPlayer;

		// CTaskSimpleUseGun::SetPedPosition()
		(( void (*)(uintptr_t, PED_TYPE *))(g_libGTASA+0x46D6AC+1))(thiz, pPed);
		
		// restore the camera modes, internal id and local player's aim
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;
		
		GameSetLocalPlayerSkills();
		
		// remote the local player's camera zoom factor
		GameSetLocalPlayerCameraExtZoomAndAspect();

		*pbyteCurrentPlayer = 0;
		GameSetLocalPlayerAim();
	} else {
		// local player

		// CTaskSimpleUseGun::SetPedPosition()
		(( void (*)(uintptr_t, PED_TYPE*))(g_libGTASA + 0x46D6AC + 1))(thiz, pPed);
	}
}

uint32_t CPad__TaskProcess(uintptr_t thiz, uintptr_t ped, int unk, int unk1)
{
	dwCurPlayerActor = ped;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);
	uint8_t old = *(uint8_t*)(g_libGTASA + 0x008E864C);
	*(uint8_t*)(g_libGTASA + 0x008E864C) = byteCurPlayer;

	uint32_t result =  ((uint32_t(*)(uintptr_t, uintptr_t, int, int))(g_libGTASA + 0x004C2F7C + 1))(thiz, ped, unk, unk1);
	*(uint8_t*)(g_libGTASA + 0x008E864C) = old;
	return result;
}

// useless shit
/*uint32_t (*CPedIntelligence__GetTaskDuck)(uintptr_t thiz, int a2);
uint32_t CPedIntelligence__GetTaskDuck_hook(uintptr_t thiz, int a2)
{
	uint32_t result;
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		result = CPedIntelligence__GetTaskDuck(thiz, a2);
		if(!result && IsRemotePlayerDucking(byteCurPlayer) &&
		IsRemotePlayerDuckingReset(byteCurPlayer) == 0)
		{
			SetRemotePlayerDuckingReset(byteCurPlayer, 1);
		}
	}
	else
	{
		uint32_t result = CPedIntelligence__GetTaskDuck(thiz, a2);
		if(result) SetLocalPlayerDucking(1);
	}
	return result;
}*/

int (*CPad__GetNitroFired)(uintptr_t thiz);
int CPad__GetNitroFired_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwCurPlayerActor;
		// local ped
		if (pPedPlayer == GamePool_FindPlayerPed())
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					return CPad__GetNitroFired(thiz);
				}
			}
		}
	}
	return 0;
}

int (*CPad__GetHydraulicJump)(uintptr_t thiz);
int CPad__GetHydraulicJump_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwCurPlayerActor;
		// local ped
		if (pPedPlayer == GamePool_FindPlayerPed())
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					return CPad__GetHydraulicJump(thiz);
				}
			}
		}
	}
	return 0;
}

int (*CPad__GetTurretLeft)(uintptr_t thiz);
int CPad__GetTurretLeft_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwCurPlayerActor;
		// local ped
		if (pPedPlayer == GamePool_FindPlayerPed())
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					return CPad__GetTurretLeft(thiz);
				}
			}
		}
	}
	return 0;
}

int (*CPad__GetTurretRight)(uintptr_t thiz);
int CPad__GetTurretRight_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwCurPlayerActor;
		// local ped
		if (pPedPlayer == GamePool_FindPlayerPed())
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					return CPad__GetTurretRight(thiz);
				}
			}
		}
	}
	return 0;
}

void (*CWidgetButtonSprint__Update)(uintptr_t);
void CWidgetButtonSprint__Update_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwCurPlayerActor;
		// local ped
		if (pPedPlayer == GamePool_FindPlayerPed())
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					return;
				}
			}
		}
	}

	CWidgetButtonSprint__Update(thiz);
}

uint32_t (*CPad__CycleWeaponLeftJustDown)(uintptr_t thiz);
uint32_t CPad__CycleWeaponLeftJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0)) return 0;
	return CPad__CycleWeaponLeftJustDown(thiz);
}

uint32_t (*CPad__CycleWeaponRightJustDown)(uintptr_t thiz);
uint32_t CPad__CycleWeaponRightJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0)) return 0;
	return CPad__CycleWeaponRightJustDown(thiz);
}

uint32_t(*CPad__GetTarget)(uintptr_t thiz, int a2, int a3, int a4);
uint32_t CPad__GetTarget_hook(uintptr_t thiz, int a2, int a3, int a4)
{
	if(!dwCurWeaponProcessingPlayer) return 0;
	if(!(*(uint32_t*)(dwCurWeaponProcessingPlayer + 1088))) return 0;
	return *(uint8_t*)(*(uint32_t*)(dwCurWeaponProcessingPlayer + 1088) + 52) & 0b00001000;
}

bool (*CPad__GetEnterTargeting)(uintptr_t thiz);
bool CPad__GetEnterTargeting_hook(uintptr_t thiz)
{
	uint8_t byteCurrentPlayer = byteCurPlayer;
	if(dwCurPlayerActor)
	{
		if(byteCurPlayer != 0)
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE];

		byteCurrentPlayer = 0;
	}

	uint8_t old = *pbyteCurrentPlayer;
	*pbyteCurrentPlayer = byteCurrentPlayer;

	uint32_t result = CPad__GetEnterTargeting(thiz);
	LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = result ? 1 : 0;

	*pbyteCurrentPlayer = old;

	return LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE];
}

uint32_t (*CCamera_IsTargetingActive)(uintptr_t thiz);
uint32_t CCamera_IsTargetingActive_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
		//return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE]; // НЕ ТУПИ ГОР :)))
		return CCamera_IsTargetingActive(thiz);
	else
	{
		*pbyteCurrentPlayer = 0;
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = CCamera_IsTargetingActive(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE];
	}
}

uint32_t (*CPad__GetExitTargeting)(uintptr_t thiz);
uint32_t CPad__GetExitTargeting_hook(uintptr_t thiz)
{
	uint8_t byteCurrentPlayer = byteCurPlayer;
	if(dwCurPlayerActor)
	{
		if(byteCurPlayer != 0)
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE];

		byteCurrentPlayer = 0;
	}

	uint8_t old = *pbyteCurrentPlayer;
	*pbyteCurrentPlayer = byteCurrentPlayer;

	uint32_t result = CPad__GetExitTargeting(thiz);
	LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = result ? 1 : 0;

	*pbyteCurrentPlayer = old;

	return LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE];
}

bool (*CPad__GetWeapon)(uintptr_t thiz, PED_TYPE* pPed);
bool CPad__GetWeapon_hook(uintptr_t thiz, PED_TYPE *pPed)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0)) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	} else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__GetWeapon(thiz, pPed);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uintptr_t (*CTaskSimplePlayerOnFoot__ProcessPlayerWeapon)(uintptr_t thiz, uintptr_t player_ped);
uintptr_t CTaskSimplePlayerOnFoot__ProcessPlayerWeapon_hook(uintptr_t thiz, uintptr_t player_ped)
{
	dwCurWeaponProcessingPlayer = player_ped;
	uintptr_t result;
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		*pbyteCurrentPlayer = byteCurPlayer;
		result = CTaskSimplePlayerOnFoot__ProcessPlayerWeapon(thiz, dwCurPlayerActor);
		*pbyteCurrentPlayer = 0;
	}
	else
	{
		result = CTaskSimplePlayerOnFoot__ProcessPlayerWeapon(thiz, dwCurPlayerActor);
	}
	return result;
}

void HookCPad()
{
	memset(&LocalPlayerKeys, 0, sizeof(PAD_KEYS));

	//CPatch::InlineHook(g_libGTASA, 0x276F80, &CWidgetButtonSprint__Update_hook, &CWidgetButtonSprint__Update);

	CPatch::InlineHook(g_libGTASA, 0x39D54C, &CPad__GetNitroFired_hook, &CPad__GetNitroFired);
	CPatch::InlineHook(g_libGTASA, 0x39D528, &CPad__GetHydraulicJump_hook, &CPad__GetHydraulicJump);
	CPatch::InlineHook(g_libGTASA, 0x39D344, &CPad__GetTurretLeft_hook, &CPad__GetTurretLeft);
	CPatch::InlineHook(g_libGTASA, 0x39D368, &CPad__GetTurretRight_hook, &CPad__GetTurretRight);
	CPatch::InlineHook(g_libGTASA, 0x45A280, &CPed__ProcessControl_hook, &CPed__ProcessControl);

    CPatch::MethodHook(g_libGTASA, 0x5CCA1C, &AllVehicles__ProcessControl_hook); // CAutomobile::ProcessControl
    CPatch::MethodHook(g_libGTASA, 0x5CCD74, &AllVehicles__ProcessControl_hook); // CBoat::ProcessControl
    CPatch::MethodHook(g_libGTASA, 0x5CCB44, &AllVehicles__ProcessControl_hook); // CBike::ProcessControl
    CPatch::MethodHook(g_libGTASA, 0x5CD0DC, &AllVehicles__ProcessControl_hook); // CPlane::ProcessControl
    CPatch::MethodHook(g_libGTASA, 0x5CCE8C, &AllVehicles__ProcessControl_hook); // CHeli::ProcessControl
    CPatch::MethodHook(g_libGTASA, 0x5CCC5C, &AllVehicles__ProcessControl_hook); // CBmx::ProcessControl
    CPatch::MethodHook(g_libGTASA, 0x5CCFB4, &AllVehicles__ProcessControl_hook); // CMonsterTruck::ProcessControl
    CPatch::MethodHook(g_libGTASA, 0x5CD204, &AllVehicles__ProcessControl_hook); // CQuadBike::ProcessControl
    CPatch::MethodHook(g_libGTASA, 0x5CD454, &AllVehicles__ProcessControl_hook); // CTrain::ProcessControl
    CPatch::MethodHook(g_libGTASA, 0x5C8610, &CTaskSimpleUseGun__SetPedPosition_hook);				 // CPad::TaskUseGun
	CPatch::MethodHook(g_libGTASA, 0x5CC1D4, &CPad__TaskProcess);  			     // CPad::TaskProcess

	CPatch::InlineHook(g_libGTASA, 0x39D08C, &CPad__GetPedWalkLeftRight_hook, &CPad__GetPedWalkLeftRight);
	CPatch::InlineHook(g_libGTASA, 0x39D110, &CPad__GetPedWalkUpDown_hook, &CPad__GetPedWalkUpDown);

	// sprint/jump stuff
	CPatch::InlineHook(g_libGTASA, 0x39EAA4, &CPad__GetSprint_hook, &CPad__GetSprint);
	CPatch::InlineHook(g_libGTASA, 0x39E9B8, &CPad__JumpJustDown_hook, &CPad__JumpJustDown);
	CPatch::InlineHook(g_libGTASA, 0x39E96C, &CPad__GetJump_hook, &CPad__GetJump);
	CPatch::InlineHook(g_libGTASA, 0x39E824, &CPad__GetAutoClimb_hook, &CPad__GetAutoClimb);
	CPatch::InlineHook(g_libGTASA, 0x39E8C0, &CPad__GetAbortClimb_hook, &CPad__GetAbortClimb);

	// swimm
	CPatch::InlineHook(g_libGTASA, 0x39EA0C, &CPad__DiveJustDown_hook, &CPad__DiveJustDown);
	CPatch::InlineHook(g_libGTASA, 0x39EA4C, &CPad__SwimJumpJustDown_hook, &CPad__SwimJumpJustDown);
	CPatch::InlineHook(g_libGTASA, 0x39DD9C, &CPad__MeleeAttackJustDown_hook, &CPad__MeleeAttackJustDown);
	CPatch::InlineHook(g_libGTASA, 0x39E7B0, &CPad__DuckJustDown_hook, &CPad__DuckJustDown);
	CPatch::InlineHook(g_libGTASA, 0x39DB50, &CPad__GetBlock_hook, &CPad__GetBlock);

	// steering lr/ud (incar)
	CPatch::InlineHook(g_libGTASA, 0x39C9E4, &CPad__GetSteeringLeftRight_hook, &CPad__GetSteeringLeftRight);
	CPatch::InlineHook(g_libGTASA, 0x39CBF0, &CPad__GetSteeringUpDown_hook, &CPad__GetSteeringUpDown);
	CPatch::InlineHook(g_libGTASA, 0x39DB7C, &CPad__GetAccelerate_hook, &CPad__GetAccelerate);
	CPatch::InlineHook(g_libGTASA, 0x39D938, &CPad__GetBrake_hook, &CPad__GetBrake);
	CPatch::InlineHook(g_libGTASA, 0x39D754, &CPad__GetHandBrake_hook, &CPad__GetHandBrake);
	CPatch::InlineHook(g_libGTASA, 0x39D4C8, &CPad__GetHorn_hook, &CPad__GetHorn);
	CPatch::InlineHook(g_libGTASA, 0x39DA1C, &CPad__ExitVehicleJustDown_hook, &CPad__ExitVehicleJustDown);

	CPatch::InlineHook(g_libGTASA, 0x39DCC4, &CPad__CycleWeaponLeftJustDown_hook, &CPad__CycleWeaponLeftJustDown);
	CPatch::InlineHook(g_libGTASA, 0x39DD30, &CPad__CycleWeaponRightJustDown_hook, &CPad__CycleWeaponRightJustDown);

	CPatch::InlineHook(g_libGTASA, 0x39E418, &CPad__GetTarget_hook, &CPad__GetTarget);
	CPatch::InlineHook(g_libGTASA, 0x39E498, &CPad__GetEnterTargeting_hook, &CPad__GetEnterTargeting);
//	CPatch::InlineHook(g_libGTASA, 0x39E230, &CPad__GetExitTargeting_hook, &CPad__GetExitTargeting);
	CPatch::InlineHook(g_libGTASA, 0x37440C, &CCamera_IsTargetingActive_hook, &CCamera_IsTargetingActive);

	CPatch::InlineHook(g_libGTASA, 0x39E038, &CPad__GetWeapon_hook, &CPad__GetWeapon);
	CPatch::InlineHook(g_libGTASA, 0x4C1748, &CTaskSimplePlayerOnFoot__ProcessPlayerWeapon_hook, &CTaskSimplePlayerOnFoot__ProcessPlayerWeapon);
}