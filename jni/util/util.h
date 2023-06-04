#pragma once
#include "../main.h"
#include "../debug.h"
#include "../chatwindow.h"
#include "../gui/gui.h"

#include "../vendor/SimpleIni/SimpleIni.h"

extern CDebug *pDebug;

uintptr_t FindLibrary(const char* library);
void cp1251_to_utf8(char *out, const char *in, unsigned int len = 0);

// Выбор сервера из пунктов меню диалога, для дальнейшего подключения
void SelectServerToJoin(uint8_t m_iSelectedItem);

template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

void Log(const char *fmt, ...);
void LogVoice(const char *fmt, ...);
void CrashLog(const char *fmt, ...);
void FLog(const char *fmt);

uint32_t GetTickCount();
std::string formatInt (unsigned int i);

bool SaveMenuSettings();

int clampEx(int source, int min, int max);
float clampEx(float source, float min, float max);

const char *GetWeaponTextureName(int iWeaponID);
