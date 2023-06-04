#pragma once

typedef struct _CAMERA_AIM
{
	float f1x, f1y, f1z;
	float pos1x, pos1y, pos1z;
	float pos2x, pos2y, pos2z;
	float f2x, f2y, f2z;
} CAMERA_AIM;

extern uint8_t *pbyteCurrentPlayer;

extern uint8_t *pbyteCameraMode;
extern uint16_t *wCameraMode2;
extern float *pfCameraExtZoom;

CAMERA_AIM* GameGetInternalAim();
uint8_t GameGetLocalPlayerCameraMode();
float GameGetAspectRatio();
float GameGetLocalPlayerCameraExtZoom();

void GameAimSyncInit();

void GameStoreLocalPlayerCameraExtZoomAndAspect();
uint8_t GameGetPlayerCameraMode(int iPlayerNum);
void GameSetRemotePlayerCameraExtZoomAndAspect(int iPlayerNum);
void GameStoreLocalPlayerAim();
void GameSetRemotePlayerAim(int iPlayerNum);
CAMERA_AIM * GameGetRemotePlayerAim(int iPlayerNum);
void GameSetLocalPlayerCameraExtZoomAndAspect();
void GameSetLocalPlayerAim();
void GameSetPlayerCameraMode(uint8_t byteMode, int iPlayerNum);
void GameStoreRemotePlayerAim(int iPlayerNum, CAMERA_AIM *pAim);
void GameSetPlayerCameraExtZoomAndAspect(int iPlayerNum, float fExtZoom, float fAspectRatio);

void GameSetLocalPlayerSkills();
void GameSetRemotePlayerSkills(int iPlayerNum);
void GameStoreLocalPlayerSkills();
void GameStoreRemotePlayerSkill(int iPlayerNum, int iSkillType, uint16_t iSkillLevel);
void GameUpdateLocalPlayerSkill(int iSkillType, uint16_t iSkillLevel);