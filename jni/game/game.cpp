#include "../main.h"
#include "../game/game.h"
#include "../util/patch.h"
#include "../gui/interface.h"
#include "loadingscreen.h"
#include "../gui/gamescreen.h"

extern CGameScreen *pGameScreen;
extern CLoadingScreen *pLoadingScreen;

void ApplyPatches();
void ApplyInGamePatches();
void InstallHooks();
void LoadSplashTexture();
void InitScripting();

uint16_t *wszTextDrawGXT = nullptr;

uint16_t *szGameTextMessage;
bool bUsedPlayerSlots[PLAYER_PED_SLOTS];

extern char* PLAYERS_REALLOC;

extern std::vector<ENTITY_TYPE*> AddedEntities;

bool CGame::IsMenuActive()
{
	// CTimer::m_UserPause
	return (*(uint8_t *)(g_libGTASA + 0x008C9BA3) == 1);	 
}

CGame::CGame()
{
	m_pGameCamera = new CCamera();
	m_pGamePlayer = nullptr;

	m_bClockEnabled = true;
	m_bCheckpointsEnabled = false;
	m_dwCheckpointMarker = 0;

	m_bRaceCheckpointsEnabled = 0;
	m_dwRaceCheckpointHandle = 0;
	m_dwRaceCheckpointMarker = 0;

	memset(&bUsedPlayerSlots[0], 0, PLAYER_PED_SLOTS);
}

void CGame::DisableAutoAim()
{
	// CPlayerPed::FindWeaponLockOnTarget
	CPatch::WriteMemory(g_libGTASA + 0x4568B0, "\x00\x20\xF7\x46", 4);
	// CPlayerPed::FindNextWeaponLockOnTarget
	CPatch::WriteMemory(g_libGTASA + 0x4590E4, "\x00\x20\xF7\x46", 4);
	CPatch::WriteMemory(g_libGTASA + 0x438DB4, "\x00\x20\xF7\x46", 4);
	
}

// 0.3.7
uint8_t CGame::FindFirstFreePlayerPedSlot()
{
	uint8_t x = 2;
	while(x != PLAYER_PED_SLOTS)
	{
		if(!bUsedPlayerSlots[x]) return x;
		x++;
	}

	return 0;
}

// 0.3.7
CPlayerPed* CGame::NewPlayer(int iSkin, float fX, float fY, float fZ, float fRotation, uint8_t byteCreateMarker)
{
	uint8_t bytePlayerNum = FindFirstFreePlayerPedSlot();
	if(!bytePlayerNum) return nullptr;

	CPlayerPed* pPlayerNew = new CPlayerPed(bytePlayerNum, iSkin, fX, fY, fZ, fRotation);
	if(pPlayerNew && pPlayerNew->m_pPed)
		bUsedPlayerSlots[bytePlayerNum] = true;

	//if(byteCreateMarker) (no xrefs ;( )
	return pPlayerNew;
}

CActorPed *CGame::NewActor(int iSkin, VECTOR vecPosition, float fRotation, float fHealth, bool bInvulnerable)
{
	CActorPed *pActorNew = new CActorPed(iSkin, vecPosition, fRotation, fHealth, bInvulnerable);
	if (pActorNew)
	    return pActorNew;
    else {
        Log("CGame::NewActor - Actor create error!");
        return nullptr;
    }
}

// 0.3.7
void CGame::RemovePlayer(CPlayerPed* pPlayer)
{
	if(pPlayer)
	{
		if(pPlayer->IsHaveAttachedObject())
			pPlayer->RemoveAllAttachedObjects();
		delete pPlayer;
	}

	bUsedPlayerSlots[pPlayer->m_bytePlayerNumber] = false;
}

// 0.3.7
CVehicle* CGame::NewVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation, bool bAddSiren)
{
	CVehicle *pVehicleNew = new	CVehicle(iType, fPosX, fPosY, fPosZ, fRotation, bAddSiren);
    if (pVehicleNew)
        return pVehicleNew;
    else {
        Log("CGame::NewVehicle - Vehicle create error!");
        return nullptr;
    }
}

CObject *CGame::NewObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance)
{
	CObject *pObjectNew = new CObject(iModel, fPosX, fPosY, fPosZ, vecRot, fDrawDistance);
    if (pObjectNew)
        return pObjectNew;
    else {
        Log("CGame::NewObject - Object create error! Model ID: %d", iModel);
        return nullptr;
    }
}

uint32_t CGame::CreatePickup(int iModel, int iType, float fX, float fY, float fZ, int* unk)
{
	Log("CreatePickup(%d, %d, %4.f, %4.f, %4.f)", iModel, iType, fX, fY, fZ);

	uint32_t hnd;

	if(!util::IsObjectInCdImage(iModel)) iModel = 18631;

	if(!ScriptCommand(&is_model_available, iModel))
	{
		ScriptCommand(&request_model, iModel);
		ScriptCommand(&load_requested_models);
		while(!ScriptCommand(&is_model_available, iModel))
			usleep(1000);
	}

	ScriptCommand(&create_pickup, iModel, iType, fX, fY, fZ, &hnd);

	int lol = 32 * (uint16_t)hnd;
	if(lol) lol /= 32;
	if(unk) *unk = lol;

	return hnd;
}

void CGame::InitInMenu()
{
	Log("CGame: InitInMenu");

	// LanguageScreen::English
	CPatch::CallFunction<void>(g_libGTASA + 0x25D20C + 1);
	
	// Disable shadows
	*(int32_t *)(g_libGTASA + 0x0063E424) = 0;

	ApplyPatches();
	InstallHooks();
	GameAimSyncInit();
	pLoadingScreen->LoadSplashTexture();

	szGameTextMessage = new uint16_t[0xFF];
}

void CGame::InitInGame()
{
	Log("CGame: InitInGame");
	ApplyInGamePatches();
	InitScripting();
	
	GameResetRadarColors();
}

float CGame::FindGroundZForCoord(float x, float y, float z)
{
	float fGroundZ;
	ScriptCommand(&get_ground_z, x, y, z, &fGroundZ);
	return fGroundZ;
}

// 0.3.7
uint32_t checkpoint_Marker = 0;
uint32_t checkpoint_Sphere = 0;
const SCRIPT_COMMAND create_sphere = { 0x03BC, "ffffv" }; // x, y, z, radius, var
const SCRIPT_COMMAND destroy_sphere = { 0x03BD, "i" }; // var
void CGame::SetCheckpointInformation(VECTOR *pos, VECTOR *extent)
{
	memcpy(&m_vecCheckpointPos,pos,sizeof(VECTOR));
	memcpy(&m_vecCheckpointExtent,extent,sizeof(VECTOR));

	if(checkpoint_Sphere && checkpoint_Marker) 
	{	
		DisableMarker(checkpoint_Marker);
		ScriptCommand(&destroy_sphere, checkpoint_Sphere);
		checkpoint_Marker = 0;
		checkpoint_Sphere = 0;
	}

	ScriptCommand(&create_radar_marker_without_sphere, pos->X, pos->Y, pos->Z, 0, &checkpoint_Marker);
	ScriptCommand(&set_marker_color, checkpoint_Marker, 1005);
	ScriptCommand(&show_on_radar, checkpoint_Marker, 3);

	ScriptCommand(&create_sphere, pos->X, pos->Y, pos->Z, extent->X, &checkpoint_Sphere);
}

// 0.3.7
void CGame::SetRaceCheckpointInformation(uint8_t byteType, VECTOR *pos, VECTOR *next, float fSize)
{
	memcpy(&m_vecRaceCheckpointPos,pos,sizeof(VECTOR));
	memcpy(&m_vecRaceCheckpointNext,next,sizeof(VECTOR));
	m_fRaceCheckpointSize = fSize;
	m_byteRaceType = byteType;

	if(m_dwRaceCheckpointMarker)
	{
		DisableMarker(m_dwRaceCheckpointMarker);

		m_dwRaceCheckpointMarker = NULL;

		m_dwRaceCheckpointMarker = CreateRadarMarkerIcon(0, m_vecRaceCheckpointPos.X,
			m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z, 1005, 0);
	}

	MakeRaceCheckpoint();
}

// 0.3.7
void CGame::MakeRaceCheckpoint()
{
	DisableRaceCheckpoint();

	ScriptCommand(&create_racing_checkpoint, (int)m_byteRaceType,
				m_vecRaceCheckpointPos.X, m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z,
				m_vecRaceCheckpointNext.X, m_vecRaceCheckpointNext.Y, m_vecRaceCheckpointNext.Z,
				m_fRaceCheckpointSize, &m_dwRaceCheckpointHandle);

	m_bRaceCheckpointsEnabled = true;
}

// 0.3.7
void CGame::DisableRaceCheckpoint()
{
	if (m_dwRaceCheckpointHandle)
	{
		ScriptCommand(&destroy_racing_checkpoint, m_dwRaceCheckpointHandle);
		m_dwRaceCheckpointHandle = NULL;
	}
	m_bRaceCheckpointsEnabled = false;
}

// 0.3.7
void CGame::UpdateCheckpoints()
{
	if(m_bCheckpointsEnabled) 
	{
		CPlayerPed *pPlayerPed = this->FindPlayerPed();
		if(pPlayerPed) 
		{
			if(ScriptCommand(&is_actor_near_point_3d,pPlayerPed->m_dwGTAId,
				m_vecCheckpointPos.X,m_vecCheckpointPos.Y,m_vecCheckpointPos.Z,
				m_vecCheckpointExtent.X,m_vecCheckpointExtent.Y,m_vecCheckpointExtent.Z,1))
				{
					DisableMarker(checkpoint_Marker);
					ScriptCommand(&destroy_sphere, checkpoint_Sphere);
					checkpoint_Marker = 0;
					checkpoint_Sphere = 0;
				}
			
			if (!checkpoint_Sphere)
			{
				ScriptCommand(&create_radar_marker_without_sphere, m_vecCheckpointPos.X,m_vecCheckpointPos.Y,m_vecCheckpointPos.Z, 0, &checkpoint_Marker);
				ScriptCommand(&set_marker_color, checkpoint_Marker, 1005);
				ScriptCommand(&show_on_radar, checkpoint_Marker, 3);

				ScriptCommand(&create_sphere, m_vecCheckpointPos.X,m_vecCheckpointPos.Y,m_vecCheckpointPos.Z,
								m_vecCheckpointExtent.X, &checkpoint_Sphere);
			}
		}
	}
	else if(checkpoint_Sphere && checkpoint_Marker) 
	{
		DisableMarker(checkpoint_Marker);
		ScriptCommand(&destroy_sphere, checkpoint_Sphere);
		checkpoint_Marker = 0;
		checkpoint_Sphere = 0;
	}
	
	if(m_bRaceCheckpointsEnabled) 
	{
		CPlayerPed *pPlayerPed = this->FindPlayerPed();
		if(pPlayerPed)
		{
			if (!m_dwRaceCheckpointMarker)
			{
				m_dwRaceCheckpointMarker = CreateRadarMarkerIcon(0, m_vecRaceCheckpointPos.X,
					m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z, 1005, 0);
			}
		}
	}
	else if(m_dwRaceCheckpointMarker) 
	{
		DisableMarker(m_dwRaceCheckpointMarker);
		DisableRaceCheckpoint();
		m_dwRaceCheckpointMarker = 0;
	}
}


// 0.3.7
uint32_t CGame::CreateRadarMarkerIcon(int iMarkerType, float fX, float fY, float fZ, int iColor, int iStyle)
{
	uint32_t dwMarkerID = 0;

	if(iStyle == 1) 
		ScriptCommand(&create_marker_icon, fX, fY, fZ, iMarkerType, &dwMarkerID);
	else if(iStyle == 2) 
		ScriptCommand(&create_radar_marker_icon, fX, fY, fZ, iMarkerType, &dwMarkerID);
	else if(iStyle == 3) 
		ScriptCommand(&create_icon_marker_sphere, fX, fY, fZ, iMarkerType, &dwMarkerID);
	else 
		ScriptCommand(&create_radar_marker_without_sphere, fX, fY, fZ, iMarkerType, &dwMarkerID);

	if(iMarkerType == 0)
	{
		if(iColor >= 1004)
		{
			ScriptCommand(&set_marker_color, dwMarkerID, iColor);
			ScriptCommand(&show_on_radar, dwMarkerID, 3);
		}
		else
		{
			ScriptCommand(&set_marker_color, dwMarkerID, iColor);
			ScriptCommand(&show_on_radar, dwMarkerID, 2);
		}
	}

	return dwMarkerID;
}

// 0.3.7
uint8_t CGame::GetActiveInterior()
{
	uint32_t dwRet;
	ScriptCommand(&get_active_interior, &dwRet);
	return (uint8_t)dwRet;
}

// 0.3.7
void CGame::SetWorldTime(int iHour, int iMinute)
{
	*(uint8_t*)(g_libGTASA+0x8B18A4) = (uint8_t)iMinute;
	*(uint8_t*)(g_libGTASA+0x8B18A5) = (uint8_t)iHour;
	ScriptCommand(&set_current_time, iHour, iMinute);
}

// 0.3.7
void CGame::SetWorldWeather(unsigned char byteWeatherID)
{
	*(unsigned char*)(g_libGTASA+0x9DB98E) = byteWeatherID;

	if(!m_bClockEnabled)
	{
		*(uint16_t*)(g_libGTASA+0x9DB990) = byteWeatherID;
		*(uint16_t*)(g_libGTASA+0x9DB992) = byteWeatherID;
	}
}

void CGame::ToggleThePassingOfTime(bool bOnOff)
{
	if(bOnOff)
		CPatch::WriteMemory(g_libGTASA+0x38C154, "\x2D\xE9", 2);
	else
		CPatch::WriteMemory(g_libGTASA+0x38C154, "\xF7\x46", 2);
}

// 0.3.7
void CGame::EnableClock(bool bEnable)
{
	char byteClockData[] = { '%', '0', '2', 'd', ':', '%', '0', '2', 'd', 0 };
	CPatch::UnFuck(g_libGTASA+0x599504);

	if(bEnable)
	{
		ToggleThePassingOfTime(true);
		m_bClockEnabled = true;
		memcpy((void*)(g_libGTASA+0x599504), byteClockData, 10);
	}
	else
	{
		ToggleThePassingOfTime(false);
		m_bClockEnabled = false;
		memset((void*)(g_libGTASA+0x599504), 0, 10);
	}
}

// 0.3.7
void CGame::EnableZoneNames(bool bEnable)
{
	ScriptCommand(&enable_zone_names, bEnable);
}

void CGame::DisplayWidgets(bool bDisp)
{
	*(uint16_t *)(g_libGTASA + 0x008B82A0 + 0x10C) = (bDisp ? 0 : 1);
}

// ��������
void CGame::PlaySound(int iSound, float fX, float fY, float fZ)
{
	ScriptCommand(&play_sound, fX, fY, fZ, iSound);
}

void CGame::ToggleRadar(bool iToggle)
{
	*(uint8_t*)(g_libGTASA+0x8EF36B) = (uint8_t)!iToggle;
}

void CGame::DisplayHUD(bool bDisp)
{
	if(bDisp)
	{	
		// CTheScripts11bDisplayHud
		*(uint8_t*)(g_libGTASA+0x7165E8) = 1;
		ToggleRadar(1);
	} else {
		*(uint8_t*)(g_libGTASA+0x7165E8) = 0;
		ToggleRadar(0);
	}
}

// 0.3.7
void CGame::RequestModel(unsigned int iModelID, int iLoadingStream)
{
	if(iModelID < 0 || iModelID > 20000) {
		return;
	}

	//CStreaming::RequestModel(int, int) С 0x0028EB10
	((void (*)(int32_t, int32_t))(g_libGTASA + 0x0028EB10 + 1))(iModelID, iLoadingStream);
}

// 0.3.7
void CGame::LoadRequestedModels()
{
	ScriptCommand(&load_requested_models);
}

void CGame::LoadRequestedModels(bool bOnlyPriorityRequests) {
	//CStreaming::LoadAllRequestedModels(bool) С 0x00294CB4
	((void (*)(bool))(g_libGTASA + 0x00294CB4 + 1))(bOnlyPriorityRequests);
}

// 0.3.7
uint8_t CGame::IsModelLoaded(unsigned int iModelID)
{
	if(iModelID < 0 || iModelID > 20000) {
		return false;
	}
	
	if(IS_VEHICLE_MOD(iModelID)) {
		//CStreaming::HasVehicleUpgradeLoaded(int) С 0x0028F328
		return ((bool (*)(int32_t))(g_libGTASA + 0x0028F328 + 1))(iModelID);
	} else {
		/*uintptr_t *pModelInfo = (uintptr_t *)g_libGTASA + 0x0087BF48;
		if(pModelInfo) {
			if(pModelInfo[dwModelID] != 0) {
				if(*(uintptr_t *)(pModelInfo + 0x34) != 0) {
					return true;
				}
			}
		}
		return false;*/
		return ScriptCommand(&is_model_available, (uint32_t)iModelID) ? true : false;
	}
}

// 0.3.7
void CGame::RefreshStreamingAt(float x, float y)
{
	ScriptCommand(&refresh_streaming_at, x, y);
}

// 0.3.7
void CGame::DisableTrainTraffic()
{
	ScriptCommand(&enable_train_traffic,0);
}

const char* CGame::GetWeaponName(int iWeaponID)
{
	switch (iWeaponID) {
	case WEAPON_BRASSKNUCKLE:
		return "Brass Knuckles";
	case WEAPON_GOLFCLUB:
		return "Golf Club";
	case WEAPON_NITESTICK:
		return "Nite Stick";
	case WEAPON_KNIFE:
		return "Knife";
	case WEAPON_BAT:
		return "Baseball Bat";
	case WEAPON_SHOVEL:
		return "Shovel";
	case WEAPON_POOLSTICK:
		return "Pool Cue";
	case WEAPON_KATANA:
		return "Katana";
	case WEAPON_CHAINSAW:
		return "Chainsaw";
	case WEAPON_DILDO:
		return "Dildo";
	case WEAPON_DILDO2:
		return "Dildo";
	case WEAPON_VIBRATOR:
		return "Vibrator";
	case WEAPON_VIBRATOR2:
		return "Vibrator";
	case WEAPON_FLOWER:
		return "Flowers";
	case WEAPON_CANE:
		return "Cane";
	case WEAPON_GRENADE:
		return "Grenade";
	case WEAPON_TEARGAS:
		return "Teargas";
	case WEAPON_MOLTOV:
		return "Molotov";
	case WEAPON_COLT45:
		return "Colt 45";
	case WEAPON_SILENCED:
		return "Silenced Pistol";
	case WEAPON_DEAGLE:
		return "Desert Eagle";
	case WEAPON_SHOTGUN:
		return "Shotgun";
	case WEAPON_SAWEDOFF:
		return "Sawn-off Shotgun";
	case WEAPON_SHOTGSPA: // wtf? 
		return "Combat Shotgun";
	case WEAPON_UZI:
		return "UZI";
	case WEAPON_MP5:
		return "MP5";
	case WEAPON_AK47:
		return "AK47";
	case WEAPON_M4:
		return "M4";
	case WEAPON_TEC9:
		return "TEC9";
	case WEAPON_RIFLE:
		return "Rifle";
	case WEAPON_SNIPER:
		return "Sniper Rifle";
	case WEAPON_ROCKETLAUNCHER:
		return "Rocket Launcher";
	case WEAPON_HEATSEEKER:
		return "Heat Seaker";
	case WEAPON_FLAMETHROWER:
		return "Flamethrower";
	case WEAPON_MINIGUN:
		return "Minigun";
	case WEAPON_SATCHEL:
		return "Satchel Explosives";
	case WEAPON_BOMB:
		return "Bomb";
	case WEAPON_SPRAYCAN:
		return "Spray Can";
	case WEAPON_FIREEXTINGUISHER:
		return "Fire Extinguisher";
	case WEAPON_CAMERA:
		return "Camera";
	case WEAPON_PARACHUTE:
		return "Parachute";
	case WEAPON_VEHICLE:
		return "Vehicle";
	case WEAPON_DROWN:
		return "Drowned";
	case WEAPON_COLLISION:
		return "Splat";
	}

	return "Unknown";
}

// 0.3.7
void CGame::SetMaxStats()
{
	// CCheat::VehicleSkillsCheat
	(( int (*)())(g_libGTASA+0x2BAED0+1))();

	// CCheat::WeaponSkillsCheat
	(( int (*)())(g_libGTASA+0x2BAE68+1))();

	// CStats::SetStatValue nop
	CPatch::WriteMemory(g_libGTASA+0x3B9074, "\xF7\x46", 2);
}

void CGame::SetWantedLevel(uint8_t byteLevel)
{
	CPatch::WriteMemory(g_libGTASA+0x27D8D2, (uintptr_t)&byteLevel, 1);
    pGameScreen->GetInterface()->SetWantedLevel(byteLevel);
}

bool CGame::IsAnimationLoaded(char *szAnimFile)
{
	return ScriptCommand(&is_animation_loaded, szAnimFile);
}

void CGame::RequestAnimation(char *szAnimFile)
{
	ScriptCommand(&request_animation, szAnimFile);
}

// 0.3.7
void CGame::DisplayGameText(char* szStr, int iTime, int iType)
{
	ScriptCommand(&text_clear_all);

	memset(szGameTextMessage, 0, sizeof(szGameTextMessage));
	CFont::AsciiToGxtChar(szStr, szGameTextMessage);

	// CMessages::AddBigMesssage
	(( void (*)(uint16_t*, int, int))(g_libGTASA+0x4D18C0+1))(szGameTextMessage, iTime, iType);
}

// 0.3.7
void CGame::SetGravity(float fGravity)
{
    CPatch::UnFuck(g_libGTASA+0x3A0B64);
	*(float*)(g_libGTASA+0x3A0B64) = fGravity;
}

void CGame::ToggleCJWalk(bool bUseCJWalk)
{
	if(bUseCJWalk)
        CPatch::WriteMemory(g_libGTASA+0x45477E, "\xC4\xF8\xDC\x64", 4);
	else
        CPatch::NOP(g_libGTASA+0x45477E, 2);
}

void CGame::DisableMarker(uint32_t dwMarkerID)
{
	ScriptCommand(&disable_marker, dwMarkerID);
}

// 0.3.7
int CGame::GetLocalMoney()
{
	return util::game::GetLocalPlayerInfo()->nHaveMoney;
}

// 0.3.7
void CGame::AddToLocalMoney(int iAmmount)
{	
	Log("MONEY: %d", iAmmount);
	ScriptCommand(&add_to_player_money, 0, iAmmount);
}

// 0.3.7
void CGame::ResetLocalMoney()
{
	int iMoney = GetLocalMoney();
	if(!iMoney) return;

	if(iMoney < 0)
		AddToLocalMoney(abs(iMoney));
	else
		AddToLocalMoney(-(iMoney));
}

// 0.3.7
void CGame::DisableInteriorEnterExits()
{
	uintptr_t addr = *(uintptr_t*)(g_libGTASA+0x700120);
	int count = *(uint32_t*)(addr+8);
	Log("Count = %d", count);

	addr = *(uintptr_t*)addr;

	for(int i=0; i<count; i++)
	{
		*(uint16_t*)(addr+0x30) = 0;
		addr += 0x3C;
	}
}

extern uint8_t bGZ;
void CGame::DrawGangZone(float fPos[], uint32_t dwColor)
{
    (( void (*)(float*, uint32_t*, uint8_t))(g_libGTASA+0x3DE7F8+1))(fPos, &dwColor, bGZ);
}

// Other stuff
void CGame::DisplayFPS()
{
	// int DisplayFPS(void)
	((int(*)())(g_libGTASA+0x39A0C4+1))();
}

int CGame::GetScreenWidth()
{
	return ((int(*)())(g_libGTASA+0x23816C+1))();
}

int CGame::GetScreenHeight()
{
	return ((int(*)())(g_libGTASA+0x238184+1))();
}

void CGame::RemoveModel(int iModel, bool bFromStreaming)
{
	if (iModel >= 0 && iModel < 20000)
	{
		if (bFromStreaming)
		{
			if(ScriptCommand(&is_model_available, iModel))
				// CStreaming::RemoveModel
				((void(*)(int))(g_libGTASA + 0x290C4C + 1))(iModel);
		}
		else
		{
			if (ScriptCommand(&is_model_available, iModel))
				ScriptCommand(&release_model);
		}
	}
}

void CGame::RemoveBuilding(uint32_t dwModel, VECTOR vecPosition, float fRange)
{
	OBJECT_REMOVE objectToRemove;
	objectToRemove.dwModel = dwModel;
	objectToRemove.vecPosition = vecPosition;
	objectToRemove.fRange = fRange * 2.0f;

	m_vecObjectToRemove.push_back(objectToRemove);

	ScriptCommand(&create_static_object, vecPosition.X, vecPosition.Y, vecPosition.Z, (fRange * 2), dwModel, 0);
}

bool CGame::IsGamePaused()
{
    return *((int *) g_libGTASA + 0x8C9BA3) != 0;
}

bool CGame::IsPlayingGame()
{
    return ((int (*)()) (g_libGTASA + 0x26A524))() != 0;
}

uint32_t CGame::GetFPS()
{
    return *(uint32_t*)(g_libGTASA + 0x8C9BA8);
}