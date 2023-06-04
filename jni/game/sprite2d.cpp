#include "../main.h"
#include "../game/sprite2d.h"
#include "../util/patch.h"

CSprite2d::CSprite2d()
{
	((void (*)(CSprite2d *))(g_libGTASA + 0x00551810 + 1))(this);  
}

CSprite2d::~CSprite2d()
{
	((void (*)(CSprite2d *))(g_libGTASA + 0x00551838 + 1))(this);  
}

void CSprite2d::Draw(float x, float y, float width, float height, CRGBA const& color)
{
	((void (*)(CSprite2d *, float, float, float, float, CRGBA))(g_libGTASA + 0x005525F8 + 1))(this, x, y, width, height, color);  
}

void CSprite2d::Delete()
{
	((void (*)(CSprite2d *))(g_libGTASA + 0x00551820 + 1))(this);
}

void CSprite2d::InitPerFrame()
{
	CPatch::CallFunction<void>(g_libGTASA + 0x5519C8 + 1);
}
	
