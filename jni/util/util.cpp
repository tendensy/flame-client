#include "../util/util.h"
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "../gui/buttons.h"
#include "../gui/interface.h"
#include "../gui/gamescreen.h"
#include "game/common.h"
#include "keyboard.h"
#include "../game/radar.h"

extern CGUI *pGUI;
extern CChatWindow *pChatWindow;
extern CGameScreen* pGameScreen;
extern CKeyBoard *pKeyBoard;

uintptr_t FindLibrary(const char* library)
{
    char filename[0xFF] = {0},
    buffer[2048] = {0};
    FILE *fp = 0;
    uintptr_t address = 0;

    sprintf( filename, "/proc/%d/maps", getpid() );

    fp = fopen( filename, "rt" );
    if(fp == 0)
    {
        Log("ERROR: can't open file %s", filename);
        goto done;
    }

    while(fgets(buffer, sizeof(buffer), fp))
    {
        if( strstr( buffer, library ) )
        {
            address = (uintptr_t)strtoul( buffer, 0, 16 );
            break;
        }
    }

    done:

    if(fp)
      fclose(fp);

    return address;
}

void cp1251_to_utf8(char *out, const char *in, unsigned int len)
{
    static const int table[128] =
    {
        // 80
        0x82D0,     0x83D0,     0x9A80E2,   0x93D1,     0x9E80E2,   0xA680E2,   0xA080E2,   0xA180E2,
        0xAC82E2,   0xB080E2,   0x89D0,     0xB980E2,   0x8AD0,     0x8CD0,     0x8BD0,     0x8FD0,
        // 90
        0x92D1,     0x9880E2,   0x9980E2,   0x9C80E2,   0x9D80E2,   0xA280E2,   0x9380E2,   0x9480E2,
        0,          0xA284E2,   0x99D1,     0xBA80E2,   0x9AD1,     0x9CD1,     0x9BD1,     0x9FD1,
        // A0
        0xA0C2,     0x8ED0,     0x9ED1,     0x88D0,     0xA4C2,     0x90D2,     0xA6C2,     0xA7C2,
        0x81D0,     0xA9C2,     0x84D0,     0xABC2,     0xACC2,     0xADC2,     0xAEC2,     0x87D0,
        // B0
        0xB0C2,     0xB1C2,     0x86D0,     0x96D1,     0x91D2,     0xB5C2,     0xB6C2,     0xB7C2,
        0x91D1,     0x9684E2,   0x94D1,     0xBBC2,     0x98D1,     0x85D0,     0x95D1,     0x97D1,
        // C0
        0x90D0,     0x91D0,     0x92D0,     0x93D0,     0x94D0,     0x95D0,     0x96D0,     0x97D0,
        0x98D0,     0x99D0,     0x9AD0,     0x9BD0,     0x9CD0,     0x9DD0,     0x9ED0,     0x9FD0,
        // D0
        0xA0D0,     0xA1D0,     0xA2D0,     0xA3D0,     0xA4D0,     0xA5D0,     0xA6D0,     0xA7D0,
        0xA8D0,     0xA9D0,     0xAAD0,     0xABD0,     0xACD0,     0xADD0,     0xAED0,     0xAFD0,
        // E0
        0xB0D0,     0xB1D0,     0xB2D0,     0xB3D0,     0xB4D0,     0xB5D0,     0xB6D0,     0xB7D0,
        0xB8D0,     0xB9D0,     0xBAD0,     0xBBD0,     0xBCD0,     0xBDD0,     0xBED0,     0xBFD0,
        // F0
        0x80D1,     0x81D1,     0x82D1,     0x83D1,     0x84D1,     0x85D1,     0x86D1,     0x87D1,
        0x88D1,     0x89D1,     0x8AD1,     0x8BD1,     0x8CD1,     0x8DD1,     0x8ED1,     0x8FD1
    };

    int count = 0;

    while (*in)
    {
        if(len && (count >= len)) break;

        if (*in & 0x80)
        {
            register int v = table[(int)(0x7f & *in++)];
            if (!v)
                continue;
            *out++ = (char)v;
            *out++ = (char)(v >> 8);
            if (v >>= 16)
                *out++ = (char)v;
        }
        else // ASCII
            *out++ = *in++;

        count++;
    }

    *out = 0;
}

void CrashLog(const char *fmt, ...)
{	
	char buffer[0xFF];
	static FILE* flLog = nullptr;

	if(flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, "%sSAMP/crashlog.txt", g_pszStorage);
		flLog = fopen(buffer, "a");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	__android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

	//if(pDebug) pDebug->AddMessage(buffer);

	char buff[80];
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	char* format = "[%I:%M:%S]";
	strftime(buff, 80, format, timeinfo);

	if(flLog == nullptr) return;
	fprintf(flLog, "%s %s\n", buff, buffer);
	fflush(flLog);

	return;
}

void Log(const char *fmt, ...)
{
    char buffer[0xFF];
    static FILE* flLog = nullptr;

    memset(buffer, 0, sizeof(buffer));

    va_list arg;
            va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
            va_end(arg);

    if(flLog == nullptr && g_pszStorage != nullptr)
    {
        sprintf(buffer, "%sSAMP/crmp.log", g_pszStorage);

        flLog = fopen(buffer, "w");
    }

    __android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

    if(pDebug) pDebug->AddMessage(buffer);

    if(flLog == nullptr)
        return;

    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);
}

void LogVoice(const char *fmt, ...)
{   
    char buffer[0xFF];
    static FILE* flLog = nullptr;

    memset(buffer, 0, sizeof(buffer));

    va_list arg;
            va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
            va_end(arg);

    if(flLog == nullptr && g_pszStorage != nullptr)
    {
        sprintf(buffer, "%sSAMP/sampvoice.txt", g_pszStorage);

        flLog = fopen(buffer, "w");
    }

    __android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

    if(pDebug) pDebug->AddMessage(buffer);

    if(flLog == nullptr)
        return;

    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);
}

int clampEx(int source, int min, int max)
{
    if(source < min) return min;
    if(source > max) return max;
    return source;
}

float clampEx(float source, float min, float max)
{
    if(source < min) return min;
    if(source > max) return max;
    return source;
}

void FLog(const char *fmt)
{
    char buffer[0xFF];
    static FILE* flLog = nullptr;
    char date[80];

    if(flLog == nullptr && g_pszStorage != nullptr)
    {
        sprintf(buffer, "%sSAMP/scripts/lua.log", g_pszStorage);

        flLog = fopen(buffer, "w");
    }

    time_t rt;
    struct tm* ti;
    time(&rt);
    ti = localtime(&rt);

    sprintf(date, "%02i:%02i:%02i", ti->tm_hour, ti->tm_min, ti->tm_sec);
    sprintf(buffer, "%s | %s", date, fmt);

    __android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

    if(pDebug) pDebug->AddMessage(buffer);

    if(flLog == nullptr)
        return;


    fprintf(flLog, "%s\n", buffer);
    fflush(flLog);
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec*1000+tv.tv_usec/1000);
}

#include "../settings.h"
extern CSettings* pSettings;
bool SaveMenuSettings()
{
    char buff[0x7F];
    sprintf(buff, "%sSAMP/crmp_settings.ini", g_pszStorage);

    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile(buff);
    SI_Error rc;

    rc = ini.SetValue("client", "name", pSettings->Get().szNickName);
    rc = ini.SetBoolValue("client", "timestamp", pGUI->timestamp);
    rc = ini.SetBoolValue("client", "radar", pSettings->Get().szRadar);
    rc = ini.SetBoolValue("client", "ChatShadow", pSettings->Get().iChatShadow);
    rc = ini.SetBoolValue("client", "textbg", pGUI->m_bRenderTextBg);
    rc = ini.SetBoolValue("client", "chatbg", pGUI->m_bRenderCBbg);
    //rc = ini.SetDoubleValue("client", "btn_style", pGameScreen->GetButtons()->btnID);
    //rc = ini.SetDoubleValue("client", "hud_style", pGameScreen->GetInterface()->GetHudID());
    rc = ini.SetBoolValue("client", "label_background", pGUI->bLabelBackground);
    rc = ini.SetBoolValue("client", "noFX", pSettings->Get().bnoFX);
    rc = ini.SetDoubleValue("client", "DrawDistPed", pSettings->Get().fDrawPedDist);
    rc = ini.SetDoubleValue("client", "texlimit", pSettings->Get().iTextureLimit);
    rc = ini.SetDoubleValue("client", "fistclick", pSettings->Get().iFistClick);
    rc = ini.SetBoolValue("client", "newbuttons", pSettings->Get().iNewButtons);
    rc = ini.SetBoolValue("client", "newhud", pSettings->Get().iHUD);
    rc = ini.SetBoolValue("client", "radar_alpha", pSettings->Get().iRadarAlpha);
    rc = ini.SetBoolValue("client", "conn_ind", pSettings->Get().szConnectIndicator);
    rc = ini.SetBoolValue("gui", "VoiceChatEnable", pSettings->Get().bVoiceChatEnable);
    rc = ini.SetDoubleValue("gui", "keyboard", pSettings->Get().iKeyboard);
    rc = ini.SetBoolValue("gui", "cutout", pSettings->Get().iCutout);
    rc = ini.SetBoolValue("gui", "fpscounter", pSettings->Get().iFPSCounter);
	
	if(pSettings->Get().iKeyboard == 2) pKeyBoard->EnableNewKeyboard();
	else pKeyBoard->EnableOldKeyboard();

	if(pSettings->Get().szRadar == true) CRadarRect::SetEnabled(true);
	else CRadarRect::SetEnabled(false);

	if(pSettings->Get().iTextureLimit != 0)
	{
		/*CPatch::UnFuck(g_libGTASA + 0x5DE734);
		*(uint32_t*)(g_libGTASA + 0x5DE734) = 0x20000000;*/				//250 mb
		if(pSettings->Get().iTextureLimit == 1) memcpy((uint32_t*)(g_libGTASA+0x5DE734), "0x10000000", 10); // CStreaming::ms_memoryAvailable(limit);
												//500 mb
		if(pSettings->Get().iTextureLimit == 2) memcpy((uint32_t*)(g_libGTASA+0x5DE734), "0x20000000", 10); // CStreaming::ms_memoryAvailable(limit);
	}
	
    rc = ini.SaveFile(buff);

    return (rc >= 0);
}

std::string formatInt (unsigned int i) {
    std::string result = to_string(i);
    int length = 0;

    if ((length = result.length()) > 3)
    {
        for (int idx = length, l = 0; --idx >= 0; l++)
        {
            if ((l > 0) && (l % 3 == 0)) result.insert(idx + 1, ","); // --> strins(value, ",", idx + 1); -> Pawn
        }
    }

    return result;
}

const char *GetWeaponTextureName(int iWeaponID)
{
    switch (iWeaponID) {
        case WEAPON_BRASSKNUCKLE:   return "fh1_kastet";
        case WEAPON_GOLFCLUB:       return "fh1_golf";
        case WEAPON_NITESTICK:      return "fh1_nitestick";
        case WEAPON_KNIFE:          return "fh1_knife";
        case WEAPON_BAT:            return "fh1_bat";
        case WEAPON_SHOVEL:         return "fh1_shovel";
        case WEAPON_POOLSTICK:      return "fh1_poolstick";
        case WEAPON_KATANA:         return "fh1_katana";
        case WEAPON_CHAINSAW:       return "fh1_chainsaw";
        case WEAPON_DILDO:          return "fh1_dildo";
        case WEAPON_DILDO2:         return "fh1_dildo2";
        case WEAPON_VIBRATOR:       return "fh1_vibrator";
        case WEAPON_VIBRATOR2:      return "fh1_vibrator2";
        case WEAPON_FLOWER:         return "fh1_flower";
        case WEAPON_CANE:           return "fh1_cane";
        case WEAPON_GRENADE:        return "fh1_grenade";
        case WEAPON_TEARGAS:        return "fh1_teargas";
        case WEAPON_MOLTOV:         return "fh1_molotov";
        case WEAPON_COLT45:         return "fh1_colt";
        case WEAPON_SILENCED:       return "fh1_silenced";
        case WEAPON_DEAGLE:         return "fh1_desert_eagle";
        case WEAPON_SHOTGUN:        return "fh1_shotgun";
        case WEAPON_SAWEDOFF:       return "fh1_sawedoff";
        case WEAPON_SHOTGSPA:       return "fh1_";
        case WEAPON_UZI:            return "fh1_uzi";
        case WEAPON_MP5:            return "fh1_mp5";
        case WEAPON_AK47:           return "fh1_ak47";
        case WEAPON_M4:             return "fh1_m4";
        case WEAPON_TEC9:           return "fh1_tec9";
        case WEAPON_RIFLE:          return "fh1_rifle";
        case WEAPON_SNIPER:         return "fh1_sniper";
        case WEAPON_ROCKETLAUNCHER: return "fh1_rpg";
        case WEAPON_HEATSEEKER:     return "fh1_heatseeker";
        case WEAPON_FLAMETHROWER:   return "fh1_fist"; // --> TODO -> Create icon --> --> --> --> -->
        case WEAPON_MINIGUN:        return "fh1_minigun";
        case WEAPON_SATCHEL:        return "fh1_bomb";
        case WEAPON_BOMB:           return "fh1_bomb";
        case WEAPON_SPRAYCAN:       return "fh1_spraycan";
        case WEAPON_FIREEXTINGUISHER: return "fh1_fire";
        case WEAPON_CAMERA:         return "fh1_camera";
        case WEAPON_PARACHUTE:      return "fh1_fist"; // --> TODO -> Create icon --> --> --> --> -->
    }

    return "fh1_fist";
}
