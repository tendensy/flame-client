#pragma once

#include "../game/rgba.h"
#include "../game/RW/RenderWare.h"

class CSprite2d 
{
public:
	RwTexture *m_pTexture;

	CSprite2d();
	~CSprite2d();
	
	void Draw(float x, float y, float width, float height, CRGBA const& color);
	void Delete();

	static void InitPerFrame();	
};