#pragma once
#include "RW/RenderWare.h"

struct stRect
{
	int x1;
    int y1;
    int x2;
    int y2;
};

struct stfRect
{
	float x1; 
    float y1;
    float x2;
    float y2;
};

class CLoadingScreen
{
private:
    
public:
    CLoadingScreen(/* args */);
    ~CLoadingScreen();

    void LoadSplashTexture();
    void Draw(stRect *rect, uint32_t color, uint32_t color2, RwRaster *raster = nullptr, stfRect *uv = nullptr);
    void Render();
    void RenderSplash();
    void SetLoadingText(char* szText) { szTextBar = szText; }

private:
    std::string szTextBar;
    uint32_t    dwLastUpdateTick;
    int         textID;
    const char *text;

    RwTexture *splashTexture;
};