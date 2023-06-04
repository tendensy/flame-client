#pragma once

#include "../game/RW/RenderWare.h"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_internal.h"
#include "UIUtils.h"
#include "../jniutil.h"

enum eTouchType
{
	TOUCH_POP = 1,
	TOUCH_PUSH = 2,
	TOUCH_MOVE = 3
};

class CGUI
{
public:
	CGUI();
	~CGUI();

	void Render();

	float ScaleX(float x) { return m_vecScale.x * x; }
	float ScaleY(float y) { return m_vecScale.y * y; }
	ImFont* GetFont() { return m_pFont; }
	ImFont* GetSampFont() { return m_pSampFont; }
	ImFont* LoadFont(char *font, float fontsize);
	ImFont* GetWeaponFont() { return m_pFontGTAWeap; }
	float GetFontSize() { return m_fFontSize; }

	bool OnTouchEvent(int type, bool multi, int x, int y);

	void RenderText(ImVec2& pos, ImU32 col, bool bOutline, const char* text_begin, const char* text_end = nullptr, float font_size = 0.0f);
	void RenderOverlayText(ImVec2& pos, ImU32 col, bool bOutline, const char* text_begin, const char* text_end = nullptr);
	void RenderTextWithSize(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size);
	void RenderTextDeathMessage(ImVec2& pos, ImU32 col, bool bOutline, const char* text_begin, const char* text_end = nullptr, float font_size = 0.0f, ImFont *font = NULL, bool bOutlineUseTextColor = false);

	void SetupDefaultStyle();
	void SetupKeyboardStyle();

public:

	bool radar;
	bool timestamp;
	bool m_bRenderCBbg;
	bool m_bRenderTextBg;
	bool bShowDebugLabels;
	bool bLabelBackground;

	/*uint8_t m_fuel;
	uint8_t m_RenderSpeedID;*/
	uint8_t m_CurrentExp;
	uint8_t m_ToUpExp;
	uint8_t m_Eat;

	ImFont* 	m_pFont;

                  ImGuiWindow* m_imWindow = nullptr;
	ImFont*		m_pFontGTAWeap;

public:
	void RenderFPS();
	void RenderRakNetStatistics();
	void RenderVersion();
	void RenderPosition();
	void SetupStyleColors();

                  void ShowSpeed();
	void SetHealth(float fHealth);
	int GetHealth();
	int bHealth;
	void SetEngine(int engine);
	int bEngine;
	void SetLights(int lights);
	int bLights;
	void SetDoor(int door);
	int bDoor;
	void SetMeliage(float meliage);
                  int bMeliage = 0;
                  void SetEat(float eat);
                  int GetEat();
                  int eat;
                  void SetFuel(float fuel);
	int m_fuel;

	void ScrollDialog(float x, float y);

public:
    int         m_iLastPosY;

	ImFont*		m_pSampFont;

	ImVec2		m_vecScale;
	float 		m_fFontSize;

	bool 		m_bMousePressed;
	ImVec2		m_vecMousePos;

	bool                            m_bTabStatus;
	bool 		m_bKeysStatus;

	float 		m_fButWidth;
	float 		m_fButHeight;

	bool 		m_bIsItemShow;
	bool 		m_bIsActive;

	bool		m_bNeedClearMousePos;
};