#include "../main.h"
#include "../game/RW/RenderWare.h"
#include "../game/game.h"
#include "../gui/gui.h"
#include "loadingscreen.h"

/*

   - BLACK RUSSIA LOADING OLD
   - RED

*/
extern CGUI *pGUI;

#define COLOR_WHITE				0xFFFFFFFF
#define COLOR_BLACK 			0xFF000000
#define COLOR_GRAY				0xFFA9A9A9
#define COLOR_LOADING_BLACK 	0x4D000000
#define COLOR_LOADING_WHITE 	0x4DFFFFFF
#define COLOR_YELLOW 			0xFFFFFFFF
#define COLOR_ORANGE 			0xFFB09321
#define COLOR_GRADIENT1			0xFFB09321 //Blue
#define COLOR_GRADIENT2			0xFFEDD56D
#define COLOR_GRADIENT3			0xFF4933EB //Cherry
#define COLOR_GRADIENT4			0xFF435CF4
#define COLOR_ROSE		0x990000FF
#define COLOR_BRED		0x990000FF
#define COLOR_PROJECT		0xFF0000FF
#define COLOR_PROJECT1		0xFF0000FF

#define MAX_SCHEMAS 3
uint32_t colors[MAX_SCHEMAS][2] =
{
	{ COLOR_BLACK,			COLOR_WHITE },
	{ COLOR_LOADING_BLACK, 	COLOR_LOADING_BLACK },
	{ COLOR_GRADIENT3,	COLOR_ROSE },
};

void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
void ImGui_ImplRenderWare_NewFrame();

CLoadingScreen::CLoadingScreen()
{
	dwLastUpdateTick = 0;
	textID = 0;
	text = "";
}

void CLoadingScreen::LoadSplashTexture()
{
	Log("Loading splash texture..");
	splashTexture = (RwTexture*)LoadTextureFromDB("samp", "mylogo");
}

void CLoadingScreen::Draw(stRect *rect, uint32_t color, uint32_t color2, RwRaster *raster, stfRect *uv)
{
	static RwIm2DVertex vert[4];
	const RwReal nearScreenZ 	= *(RwReal*)(g_libGTASA+0x9DAA60);	// CSprite2d::NearScreenZ
	const RwReal recipNearClip 	= *(RwReal*)(g_libGTASA+0x9DAA64);	// CSprite2d::RecipNearClip

	RwIm2DVertexSetScreenX(&vert[0], rect->x1);
	RwIm2DVertexSetScreenY(&vert[0], rect->y2);
	RwIm2DVertexSetScreenZ(&vert[0], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[0], recipNearClip);
	vert[0].emissiveColor = color;
	RwIm2DVertexSetU(&vert[0], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[0], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[1], rect->x2);
	RwIm2DVertexSetScreenY(&vert[1], rect->y2);
	RwIm2DVertexSetScreenZ(&vert[1], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[1], recipNearClip);
	vert[1].emissiveColor = color2;
	RwIm2DVertexSetU(&vert[1], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[1], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[2], rect->x1);
	RwIm2DVertexSetScreenY(&vert[2], rect->y1);
	RwIm2DVertexSetScreenZ(&vert[2], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[2], recipNearClip);
	vert[2].emissiveColor = color;
	RwIm2DVertexSetU(&vert[2], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[2], uv ? uv->y1 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[3], rect->x2);
	RwIm2DVertexSetScreenY(&vert[3], rect->y1);
	RwIm2DVertexSetScreenZ(&vert[3], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[3], recipNearClip);
	vert[3].emissiveColor = color2;
	RwIm2DVertexSetU(&vert[3], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[3], uv ? uv->y1 : 0.0f, recipNearClip);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)raster);
	RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vert, 4);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);
}

void CLoadingScreen::RenderSplash()
{
	if (!splashTexture) return;

	stRect rect;
	stfRect uv;
	stfRect sRect;

	// background
	rect.x1 = 0;
                  rect.y1 = 0;
                  rect.x2 = RsGlobal->maximumWidth;
                  rect.y2 = RsGlobal->maximumHeight;
	Draw(&rect, colors[0][0], colors[0][0]);

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	
	// splash
	rect.x1 = 0;
                  rect.y1 = 0;
                  rect.x2 = RsGlobal->maximumWidth;
                  rect.y2 = RsGlobal->maximumHeight;
                  uv.x1 = 0.0f;
                  uv.y1 = 0.0f;
                  uv.x2 = 1.0f;
                  uv.y2 = 1.0f;
	Draw(&rect, colors[0][1], colors[0][1], splashTexture->raster, &uv);

	// loading progress
	const float percent = *(float*)(g_libGTASA+0x8F08C0);
	if(percent <= 0.0f) return;
	
	float mult = percent / 100.0f; // 100.0f 60.0f
                  // fixed 100 result in loading

	rect.x1 = 0;
	rect.y1 = RsGlobal->maximumHeight * 0.9f;
	rect.x2 = RsGlobal->maximumWidth * mult;
	rect.y2 = RsGlobal->maximumHeight;
	
	Draw(&rect, COLOR_PROJECT, COLOR_PROJECT1);
}

#include "../vendor/imgui/stb_image.h"
void CLoadingScreen::Render()
{
	RenderSplash();

	/*if (!pGUI) return; 
	const float percent = *(float*)(g_libGTASA + 0x8F08C0);
	if (percent <= 0.0f) return;
	float mult = percent / 100.0f;
	// offset
                  float newX = pGUI->ScaleX(0.0f) + (mult * 3.0 * RsGlobal->maximumWidth) + pGUI->ScaleX(0.0f);*/

	ImGui_ImplRenderWare_NewFrame();
                  ImGui::NewFrame();
                  ImGuiIO& io = ImGui::GetIO();
	
	ImColor colorWhite(0xFF, 0xFF, 0xFF, 0xFF);

                  if((GetTickCount() - dwLastUpdateTick) >= 500)
                  {
                         dwLastUpdateTick = GetTickCount();

                         textID++;
                         if (textID == 3) textID = 0;

                          switch (textID) {
                                  case 0: text = " "; break;
                                  case 1: text = " "; break;
                                  case 2: text = " "; break;
                    }
    }

    ImVec2 posText(pGUI->ScaleX(70), io.DisplaySize.y - pGUI->ScaleY(70));
    pGUI->RenderOverlayText(posText, colorWhite, false, text);

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());
}
