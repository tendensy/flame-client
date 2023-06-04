#pragma once

#define MAX_SETTINGS_STRING	0x7F

struct stSettings
{
	// client
	char szNickName[MAX_PLAYER_NAME+1];
	char szHost[MAX_SETTINGS_STRING+1];
	int iPort;
	char szPassword[MAX_SETTINGS_STRING+1];
	bool szTimeStamp;
	bool szRadar;
	bool szTextBG;
	bool szChatBG;
	bool szConnectIndicator;
	bool iHUD;
	bool iNewButtons;
	bool iCoord;
	bool iRadarAlpha;
	int iBtnStyle;
	int iHudStyle;

	bool bLabelBg;
	bool bnoFX;
	float fDrawPedDist;
	int iTextureLimit;
	int iFistClick;

	// debug
	bool bDebug;
	bool bOnline;

	// gui
	char szFont[40];
	float fFontSize;
	int iFontOutline;
	float fChatPosX;
	float fChatPosY;
	float fChatSizeX;
	float fChatSizeY;
	float fScoreBoardSizeX;
	float fScoreBoardSizeY;
	int iChatMaxMessages;
	bool iChatShadow;
	float fSpawnScreenPosX;
	float fSpawnScreenPosY;
	float fSpawnScreenSizeX;
	float fSpawnScreenSizeY;
	float fHealthBarWidth;
	float fHealthBarHeight;

	bool bVoiceChatEnable;
	int iVoiceChatKey;
	float fVoiceChatSize;
	float fVoiceChatPosX;
	float fVoiceChatPosY;
	
	//bool iAndroidKeyboard;
	//bool iNewKeyboard;
	int iKeyboard;
	bool iCutout;
	bool iFPSCounter;

};

class CSettings
{
public:
	CSettings();
	~CSettings();

	stSettings& Get() { return m_Settings; }

    bool timestamp;
	bool ToggleTimeStamp();
	
private:
	struct stSettings m_Settings;
};