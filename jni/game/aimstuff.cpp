#include "main.h"
#include "game.h"

uint8_t *pbyteCurrentPlayer = nullptr;

uint8_t *pbyteCameraMode = nullptr;
uint16_t *wCameraMode2 = nullptr;
float *pfCameraExtZoom = nullptr;
float *pfAspectRatio = nullptr;
CAMERA_AIM *pcaInternalAim = nullptr;

CAMERA_AIM caLocalPlayerAim;
CAMERA_AIM caRemotePlayerAim[PLAYER_PED_SLOTS];
uint8_t byteCameraMode[PLAYER_PED_SLOTS];
float fCameraExtZoom[PLAYER_PED_SLOTS];
float fLocalCameraExtZoom;
float fCameraAspectRatio[PLAYER_PED_SLOTS];
float fLocalAspectRatio;

void GameAimSyncInit()
{
	memset(&caLocalPlayerAim, 0, sizeof(CAMERA_AIM));
	memset(caRemotePlayerAim, 0, sizeof(CAMERA_AIM) * PLAYER_PED_SLOTS);
	memset(byteCameraMode, 4, PLAYER_PED_SLOTS);
	memset(fCameraAspectRatio, 0, PLAYER_PED_SLOTS);
	memset(fCameraExtZoom, 1, PLAYER_PED_SLOTS);

	pcaInternalAim = (CAMERA_AIM*)(g_libGTASA+0x8B0AE0);
	pbyteCameraMode = (uint8_t*)(g_libGTASA+0x8B0808+0x17E);
	wCameraMode2 = (uint16_t*)(g_libGTASA+0x8B0808+0x7B4);
	pfAspectRatio = (float*)(g_libGTASA+0x98525C);
	pfCameraExtZoom = (float*)(g_libGTASA+0x8B0808+0x1FC);
	pbyteCurrentPlayer = (uint8_t*)(g_libGTASA+0x8E864C);
}

CAMERA_AIM* GameGetInternalAim()
{
	return pcaInternalAim;
}

uint8_t GameGetLocalPlayerCameraMode()
{
	return *pbyteCameraMode;
}

float GameGetAspectRatio()
{
	return *pfAspectRatio;
}

float GameGetLocalPlayerCameraExtZoom()
{
	float value = ((*pfCameraExtZoom) - 35.0f) / 35.0f;	// normalize for 70.0 to 35.0
	return value;
}

void GameStoreLocalPlayerCameraExtZoomAndAspect()
{
	fLocalCameraExtZoom = *pfCameraExtZoom;
	fLocalAspectRatio = *pfAspectRatio;
}

void GameSetRemotePlayerCameraExtZoomAndAspect(int iPlayerNum)
{
	float fZoom = fCameraExtZoom[iPlayerNum];
	float fValue = fZoom * 35.0f + 35.0f; // unnormalize for 35.0 to 70.0
	*pfCameraExtZoom = fValue;
	*pfAspectRatio = fCameraAspectRatio[iPlayerNum]+1;
}

uint8_t GameGetPlayerCameraMode(int iPlayerNum)
{
	return byteCameraMode[iPlayerNum];
}

void GameStoreLocalPlayerAim()
{
	memcpy(&caLocalPlayerAim, pcaInternalAim, sizeof(CAMERA_AIM));
}

void GameSetRemotePlayerAim(int iPlayerNum)
{
	memcpy(pcaInternalAim, &caRemotePlayerAim[iPlayerNum], sizeof(CAMERA_AIM));
}

CAMERA_AIM * GameGetRemotePlayerAim(int iPlayerNum)
{
	return &caRemotePlayerAim[iPlayerNum];
}

void GameSetLocalPlayerCameraExtZoomAndAspect()
{
	*pfCameraExtZoom = fLocalCameraExtZoom;
	*pfAspectRatio = fLocalAspectRatio;
}

void GameSetLocalPlayerAim()
{
	memcpy(pcaInternalAim, &caLocalPlayerAim, sizeof(CAMERA_AIM));
}

void GameSetPlayerCameraMode(uint8_t byteMode, int iPlayerNum)
{
	byteCameraMode[iPlayerNum] = byteMode;
}

void GameStoreRemotePlayerAim(int iPlayerNum, CAMERA_AIM* pAim)
{
	memcpy(&caRemotePlayerAim[iPlayerNum], pAim, sizeof(CAMERA_AIM));
}

void GameSetPlayerCameraExtZoomAndAspect(int iPlayerNum, float fExtZoom, float fAspectRatio)
{
	fCameraExtZoom[iPlayerNum] = fExtZoom;
	fCameraAspectRatio[iPlayerNum] = fAspectRatio;
}

void GameSetLocalPlayerSkills()
{
	
}

void GameSetRemotePlayerSkills(int iPlayerNum)
{
	
}

void GameStoreLocalPlayerSkills()
{
	
}

void GameStoreRemotePlayerSkill(int iPlayerNum, int iSkillType, uint16_t iSkillLevel)
{
	
}

void GameUpdateLocalPlayerSkill(int iSkillType, uint16_t iSkillLevel)
{
	
}