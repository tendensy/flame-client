#pragma once

#include "../game/common.h"
#include "../game/aimstuff.h"
#include "../game/pad.h"
#include "../game/quaternion.h"
#include "../game/entity.h"
#include "../game/playerped.h"
#include "../game/vehicle.h"
#include "../game/object.h"
#include "../game/camera.h"
#include "../game/font.h"
#include "../game/scripting.h"
#include "../game/radarcolors.h"
#include "../game/util.h"
#include "../game/actorped.h"
#include "../game/textdraw.h"
#include "../game/snapshothelper.h"

#include "../game/vector2d.h"
#include "../game/streaming.h"
#include "../game/sprite2d.h"

#include <vector>

class CGame
{
public:
	CGame();
	~CGame() {};

	void InitInMenu();
	void InitInGame();

	CCamera* GetCamera() { return m_pGameCamera; }
	// 0.3.7
	CPlayerPed* FindPlayerPed() { if(!m_pGamePlayer) m_pGamePlayer = new CPlayerPed(); return m_pGamePlayer; }
	// 0.3.7
	uint8_t FindFirstFreePlayerPedSlot();
	// 0.3.7
	CPlayerPed* NewPlayer(int iSkin, float fX, float fY, float fZ, float fRotation, uint8_t byteCreateMarker = 0);
	CActorPed *NewActor(int iSkin, VECTOR vecPosition, float fRotation, float fHealth, bool bInvulnerable);
	// 0.3.7
	void RemovePlayer(CPlayerPed* pPlayer);
	// 0.3.7
	CVehicle* NewVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation, bool bAddSiren);
	CObject* NewObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance);
	uint32_t CreatePickup(int iModel, int iType, float fX, float fY, float fZ, int *unk);

	const char* GetWeaponName(int iWeaponID);

	bool IsMenuActive();

	//
	void DisableAutoAim();

	float FindGroundZForCoord(float x, float y, float z);
	// 0.3.7
	uint8_t GetActiveInterior();
	// 0.3.7
	void SetWorldTime(int iHour, int iMinute);
	// 0.3.7
	void SetWorldWeather(unsigned char byteWeatherID);
	// 0.3.7
	void EnableClock(bool bEnable);
	void ToggleThePassingOfTime(bool bOnOff);
	// 0.3.7
	void EnableZoneNames(bool bEnable);
	void DisplayWidgets(bool bDisp);
	void PlaySound(int iSound, float fX, float fY, float fZ);

	// 0.3.7
	void ToggleRadar(bool iToggle);
	// 0.3.7
	void DisplayHUD(bool bDisp);

	// 0.3.7
	void ToggleCheckpoints(bool bEnabled) { m_bCheckpointsEnabled = bEnabled; }
	// 0.3.7
	void ToggleRaceCheckpoints(bool bEnabled) { m_bRaceCheckpointsEnabled = bEnabled; }
	// 0.3.7
	void SetCheckpointInformation(VECTOR *pos, VECTOR *extent);
	// 0.3.7
	void SetRaceCheckpointInformation(uint8_t byteType, VECTOR *pos, VECTOR *next, float fSize);
	// 0.3.7
	void UpdateCheckpoints();
	// 0.3.7
	void MakeRaceCheckpoint();
	// 0.3.7
	void DisableRaceCheckpoint();
	// 0.3.7
	uint32_t CreateRadarMarkerIcon(int iMarkerType, float fX, float fY, float fZ, int iColor, int iStyle);
	// 0.3.7
	void DisableMarker(uint32_t dwMarkerID);
	// 0.3.7
	void RequestModel(unsigned int iModelID, int iLoadingStream = 2);
	// 0.3.7
	void LoadRequestedModels();
	void LoadRequestedModels(bool bOnlyPriorityRequests);
	// 0.3.7
	uint8_t IsModelLoaded(unsigned int iModelID);
	// 0.3.7
	void RefreshStreamingAt(float x, float y);
	// 0.3.7
	void DisableTrainTraffic();
	// 0.3.7
	void SetMaxStats();

	void SetWantedLevel(uint8_t byteLevel);

	bool IsAnimationLoaded(char* szAnimFile);
	void RequestAnimation(char* szAnimFile);
	// 0.3.7
	void DisplayGameText(char* szStr, int iTime, int iType);
	// 0.3.7
	void SetGravity(float fGravity);
	void ToggleCJWalk(bool bUseCJWalk);
	// 0.3.7
	void DisableInteriorEnterExits();

	void RemoveModel(int iModel, bool bFromStreaming);

	// 0.3.7
	int GetLocalMoney();
	// 0.3.7
	void AddToLocalMoney(int iAmmount);
	// 0.3.7
	void ResetLocalMoney();

	void DrawGangZone(float fPos[], uint32_t dwColor);

	// Additional interesting functions
	void DisplayFPS();

	int GetScreenWidth();
	int GetScreenHeight();

	std::vector<OBJECT_REMOVE> GetObjectsToRemoveList() {
		return m_vecObjectToRemove;
	};
	void RemoveBuilding(uint32_t dwModel, VECTOR vecPosition, float fRange);

    bool IsGamePaused();
    bool IsPlayingGame();

    uint32_t GetFPS();

private:
	CCamera* 		m_pGameCamera;
	CPlayerPed*		m_pGamePlayer;

	// checkpoint
	bool			m_bCheckpointsEnabled;
	uint32_t		m_dwCheckpointMarker;
	VECTOR			m_vecCheckpointPos;
	VECTOR			m_vecCheckpointExtent;
	// race checkpoint
	bool			m_bRaceCheckpointsEnabled;
	uint32_t		m_dwRaceCheckpointHandle;
	uint32_t		m_dwRaceCheckpointMarker;
	float			m_fRaceCheckpointSize;
	uint8_t			m_byteRaceType;
	VECTOR 			m_vecRaceCheckpointPos;
	VECTOR			m_vecRaceCheckpointNext;

	bool 			m_bClockEnabled;
	std::vector<OBJECT_REMOVE>	m_vecObjectToRemove;
};