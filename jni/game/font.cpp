#include "../main.h"
#include "font.h"

extern uint16_t *wszTextDrawGXT;

void CFont::AsciiToGxtChar(const char *szASCII, uint16_t *wszGXT)
{
	((void (*)(const char *, uint16_t *))(g_libGTASA + 0x00532D00 + 1))(szASCII, wszGXT);
}

void CFont::PrintString(float fX, float fY, const char *szText)
{
	uint16_t *gxt_string = new uint16_t[800];
	AsciiToGxtChar(szText, gxt_string);
	(( void (*)(float, float, uint16_t*))(g_libGTASA+0x5353B4+1))(fX, fY, gxt_string);
	delete gxt_string;
	(( void (*)())(g_libGTASA+0x53411C+1))();//53411C ; _DWORD RenderFontBuffer(CFont *__hidden this)   
}

void CFont::SetColor(uint32_t *color)
{
	((void (*)(uint32_t*))(g_libGTASA + 0x5336F4 + 1))(color);
}

void CFont::SetDropColor(uint32_t* color)
{
	((void (*)(uint32_t*))(g_libGTASA + 0x53387C + 1))(color);
}

void CFont::SetEdge(bool on)
{
	((void (*)(bool))(g_libGTASA + 0x0053394C + 1))(on);
}

void CFont::SetJustify(bool on)
{
	((void (*)(bool))(g_libGTASA + 0x005339D0 + 1))(on);
}

void CFont::SetScale(float fValue) 
{
	((void (*)(float))(g_libGTASA + 0x00533694 + 1))(fValue);
}

void CFont::SetScaleXY(float fValueX, float fValueY) 
{
	*(float *)(g_libGTASA + 0x0099D754) = fValueY;
	*(float *)(g_libGTASA + 0x0099D750) = fValueX;
}

void CFont::SetOrientation(unsigned char ucValue) 
{	
	((void (*)(int))(g_libGTASA + 0x005339E8 + 1))(ucValue);
}

void CFont::SetFontStyle(unsigned char ucValue) 
{
	((void (*)(int))(g_libGTASA + 0x00533748 + 1))(ucValue);
}

void CFont::SetProportional(unsigned char ucValue) 
{
	((void (*)(bool))(g_libGTASA + 0x00533970 + 1))(ucValue);
}

void CFont::SetRightJustifyWrap(float fValue) 
{	
	((void (*)(float))(g_libGTASA + 0x0053384C + 1))(fValue);
}

void CFont::SetBackground(bool enable, bool includeWrap) 
{
	((void (*)(bool, bool))(g_libGTASA + 0x00533988 + 1))(enable, includeWrap);
}

void CFont::SetBackgroundColor(uint32_t *uiColor) 
{
	((void (*)(uint32_t*))(g_libGTASA + 0x5339A4 + 1))(uiColor);
}

void CFont::SetWrapx(float fValue) 
{
	((void (*)(float))(g_libGTASA + 0x0053381C + 1))(fValue);
}

void CFont::SetCentreSize(float fValue) 
{
	((void (*)(float))(g_libGTASA + 0x00533834 + 1))(fValue);
}

void CFont::SetDropShadowPosition(signed char scValue) 
{
	((void (*)(short))(g_libGTASA + 0x005338DC + 1))(scValue);
}

float CFont::GetHeight(bool unk)
{
	return ((float (*)(bool))(g_libGTASA + 0x005330F0 + 1))(unk);
}

float CFont::GetStringWidth(uint16_t* str)
{
	return ((float (*)(uint16_t*))(g_libGTASA + 0x00534BAC + 1))(str);
}
void CFont::GetTextRect(RECT rect, float x, float y)
{
	((float (*)(RECT, float, float, uint16_t *))(g_libGTASA + 0x005352DC + 1))(rect, x, y, wszTextDrawGXT);
}