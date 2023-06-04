#pragma once
#include "../game/common.h"
#include "rgba.h"

enum eFontAlignment : unsigned char {
    ALIGN_CENTER,
    ALIGN_LEFT,
    ALIGN_RIGHT
};

class CFont
{
public:
	static void AsciiToGxtChar(const char *szASCII, uint16_t *wszGXT);
	static void PrintString(float fX, float fY, const char *szText);
	static void SetColor(uint32_t *color);
	static void SetDropColor(uint32_t* color);
	static void SetEdge(bool on);
	static void SetJustify(bool on);
	static void SetScale(float fValue) ;
	static void SetScaleXY(float fValueX, float fValueY);
	static void SetOrientation(unsigned char ucValue);
	static void SetFontStyle(unsigned char ucValue);
	static void SetProportional(unsigned char ucValue);
	static void SetRightJustifyWrap(float fValue);
	static void SetBackground(bool enable, bool includeWrap);
	static void SetBackgroundColor(uint32_t *uiColor);
	static void SetWrapx(float fValue);
	static void SetCentreSize(float fValue);
	static void SetDropShadowPosition(signed char scValue);
	static float GetHeight(bool unk);
	static float GetStringWidth(uint16_t* str);
	static void GetTextRect(RECT rect, float x, float y);
};