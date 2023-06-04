#include "main.h"
#include "settings.h"
#include "vendor/inih/cpp/INIReader.h"

#include "vendor/SimpleIni/SimpleIni.h"
#include "util/patch.h"

void ApplyFPSPatch(uint8_t);

CSettings::CSettings()
{
	Log("Loading settings..");

	char buff[0x7F];
	sprintf(buff, "%sSAMP/crmp_settings.ini", g_pszStorage);

	INIReader reader(buff);

	if(reader.ParseError() < 0)
	{
		Log("Error: can't load %s", buff);
		std::terminate();
		return;
	}

	// Client
	size_t length = 0;
	length = reader.Get("client", "name", "Test_Nickname").copy(m_Settings.szNickName, MAX_PLAYER_NAME);
	m_Settings.szNickName[length] = '\0';
	length = reader.Get("client", "host", "127.0.0.1").copy(m_Settings.szHost, MAX_SETTINGS_STRING);
	m_Settings.szHost[length] = '\0';
	length = reader.Get("client", "password", "").copy(m_Settings.szPassword, MAX_SETTINGS_STRING);
	m_Settings.szPassword[length] = '\0';
	m_Settings.iPort = reader.GetInteger("client", "port", 7777);

    int fpsLimit = reader.GetInteger("client", "fps", 90);
    ApplyFPSPatch(fpsLimit);

	m_Settings.szTimeStamp = reader.GetBoolean("client", "timestamp", false);
	m_Settings.szRadar = reader.GetBoolean("client", "radar", false);
	m_Settings.szTextBG = reader.GetBoolean("client", "textbg", false);
	m_Settings.szConnectIndicator = reader.GetBoolean("client", "conn_ind", true);
	m_Settings.iHUD = reader.GetBoolean("client", "newhud", true);
	m_Settings.iNewButtons = reader.GetBoolean("client", "newbuttons", true);
	m_Settings.iRadarAlpha = reader.GetBoolean("client", "radar_alpha", false);
	m_Settings.iCoord = reader.GetBoolean("client", "coord", false);
	m_Settings.szChatBG = reader.GetBoolean("client", "chatbg", false);
	m_Settings.iBtnStyle = reader.GetInteger("client", "btn_style", 0); if(m_Settings.iBtnStyle > 15) m_Settings.iBtnStyle = 1;
	m_Settings.iHudStyle = reader.GetInteger("client", "hud_style", 0);
	m_Settings.iTextureLimit = reader.GetInteger("client", "texlimit", 1);
	m_Settings.iFistClick = reader.GetInteger("client", "fistclick", 0);

	m_Settings.bnoFX = reader.GetBoolean("client", "noFX", false);
	m_Settings.fDrawPedDist = reader.GetReal("client", "DrawDistPed", 60.0f);
	m_Settings.bLabelBg = reader.GetBoolean("client", "label_background", false);
	
	m_Settings.iChatShadow = reader.GetBoolean("client", "ChatShadow", true);

	if(m_Settings.bnoFX)
	{
		//CPatch::NOP(g_libGTASA + 0x39B36A, 2);
		CPatch::WriteMemory(g_libGTASA + 0x52DD38, "\x00\x20\x70\x47", 4); // return 0 CCoronas::RenderReflections
		CPatch::NOP(g_libGTASA + 0x39AD14, 1); // skip render clouds, sunrefl, raineffect 
	}

	// Debug
	m_Settings.bDebug = reader.GetBoolean("debug", "debug", false);
	m_Settings.bOnline = reader.GetBoolean("debug", "online", false);

	// gui
	length = reader.Get("gui", "Font", "Arial.ttf").copy(m_Settings.szFont, 35);
	m_Settings.szFont[length] = '\0';
	m_Settings.fFontSize = reader.GetReal("gui", "FontSize", 30.0f);
	m_Settings.iFontOutline = reader.GetInteger("gui", "FontOutline", 1/*2*/);
	// chat
	m_Settings.fChatPosX = reader.GetReal("gui", "ChatPosX", 325.0f);
	m_Settings.fChatPosY = reader.GetReal("gui", "ChatPosY", 25.0f);
	m_Settings.fChatSizeX = reader.GetReal("gui", "ChatSizeX", 1150.0f);
	m_Settings.fChatSizeY = reader.GetReal("gui", "ChatSizeY", 220.0f);
	m_Settings.iChatMaxMessages = reader.GetInteger("gui", "ChatMaxMessages", 8);
	// scoreboard
	m_Settings.fScoreBoardSizeX = reader.GetReal("gui", "ScoreBoardSizeX", 1024.0f);
	m_Settings.fScoreBoardSizeY = reader.GetReal("gui", "ScoreBoardSizeY", 768.0f);
	// spawnscreen
	m_Settings.fSpawnScreenPosX = reader.GetReal("gui", "SpawnScreenPosX", 660.0f);
	m_Settings.fSpawnScreenPosY = reader.GetReal("gui", "SpawnScreenPosY", 950.0f);
	m_Settings.fSpawnScreenSizeX = reader.GetReal("gui", "SpawnScreenSizeX", 600.0f);
	m_Settings.fSpawnScreenSizeY = reader.GetReal("gui", "SpawnScreenSizeY", 100.0f);
	// nametags
	m_Settings.fHealthBarWidth = reader.GetReal("gui", "HealthBarWidth", 100.0f);
	m_Settings.fHealthBarHeight = reader.GetReal("gui", "HealthBarHeight", 10.0f);

	m_Settings.bVoiceChatEnable = reader.GetBoolean("gui", "VoiceChatEnable", true);
	m_Settings.iVoiceChatKey = reader.GetInteger("gui", "VoiceChatKey", 66);
	m_Settings.fVoiceChatSize = reader.GetReal("gui", "VoiceChatSize", 30.0f);
	m_Settings.fVoiceChatPosX = reader.GetReal("gui", "VoiceChatPosX", 1520.0f);
	m_Settings.fVoiceChatPosY = reader.GetReal("gui", "VoiceChatPosY", 480.0f);
	
	//m_Settings.iAndroidKeyboard = reader.GetBoolean("gui", "androidkeyboard", false);
	m_Settings.iKeyboard = reader.GetInteger("gui", "keyboard", 0);
	m_Settings.iCutout = reader.GetBoolean("gui", "cutout", false);
	m_Settings.iFPSCounter = reader.GetBoolean("gui", "fpscounter", false);
	//m_Settings.iNewKeyboard = reader.GetBoolean("gui", "newkeyboard", false);
}

bool CSettings::ToggleTimeStamp()
{
	char buff[0x7F];
	sprintf(buff, "%sSAMP/settings.ini", g_pszStorage);

	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(buff);
	SI_Error rc;

    timestamp = Get().szTimeStamp;
    m_Settings.szTimeStamp = !timestamp;

	rc = ini.SetBoolValue("client", "timestamp", !timestamp);
	rc = ini.SaveFile(buff);

	return (rc >= 0);
}
