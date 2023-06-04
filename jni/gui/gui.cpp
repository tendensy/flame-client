#include "../main.h"

#include "gui.h"

#include "../game/game.h"
#include "../net/netgame.h"

#include "../spawnscreen.h"
#include "../playertags.h"
#include "../dialog.h"
#include "../keyboard.h"
#include "../settings.h"
#include "../scoreboard.h"
#include "../deathmessage.h"
#include "../GButton.h"
#include "../game/radar.h"

#include "../util/patch.h"

extern CJavaWrapper* pJavaWrapper;

#include "../util/util.h"
#include "../game/vehicle.h"

#include "interface.h"
#include "buttons.h"
#include "gamescreen.h"
#include "../game/firstperson.h"

#include "../debug.h"

extern CChatWindow *pChatWindow;
extern CSpawnScreen *pSpawnScreen;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CSettings *pSettings;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern CScoreBoard *pScoreBoard;
extern CDeathMessage* pDeathMessage;
extern CGame *pGame;
extern CGButton *pGButton;
extern CGameScreen *pGameScreen;
extern CDebug *pDebug;
extern CFirstPersonCamera *pFirstPersonCamera;

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_Init();
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();

/*
	Все координаты GUI-элементов задаются
	относительно разрешения 1920x1080
*/

#define MULT_X	1.0f / 1920
#define MULT_Y	1.0f / 1080

CGUI::CGUI()
{
    Log("Initializing GUI..");

    // setup ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui_ImplRenderWare_Init();

    // scale
    m_vecScale.x    = io.DisplaySize.x * MULT_X;
    m_vecScale.y    = io.DisplaySize.y * MULT_Y;
    // font Size
    m_fFontSize     = ScaleY( pSettings->Get().fFontSize );

    // mouse/touch
    m_bMousePressed = false;
    m_vecMousePos   = ImVec2(0, 0);

    Log("GUI | Scale factor: %f, %f Font size: %f", m_vecScale.x, m_vecScale.y, m_fFontSize);

    // ImGui::StyleColorsClassic();
    ImGui::StyleColorsDark();
    SetupDefaultStyle();

    m_bKeysStatus   = false;
    m_bTabStatus    = false;

    m_bRenderCBbg	= pSettings->Get().szChatBG;
    m_bRenderTextBg	= pSettings->Get().szTextBG;

//    m_RenderSpeedID = 0;

    m_CurrentExp = 1;
    m_ToUpExp = 1;
    m_Eat = 0;

    radar = pSettings->Get().szRadar;
    timestamp = pSettings->Get().szTimeStamp;

    bShowDebugLabels = false;

    //m_fuel = 0;
    bLabelBackground = pSettings->Get().bLabelBg;

    // load fonts
    char path[0xFF];
    sprintf(path, "%sSAMP/fonts/%s", g_pszStorage, pSettings->Get().szFont);
    // cp1251 ranges
    static const ImWchar ranges[] =
    {
        0x0020, 0x0080,
        0x00A0, 0x00C0,
        0x0400, 0x0460,
        0x0490, 0x04A0,
        0x2010, 0x2040,
        0x20A0, 0x20B0,
        0x2110, 0x2130,
        0
    };
    Log("GUI | Loading font: %s", pSettings->Get().szFont);
    m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
    Log("GUI | ImFont pointer = 0x%X", m_pFont);

    Log("GUI | Loading font: gtaweap3.ttf");
	m_pFontGTAWeap = LoadFont("gtaweap3.ttf", 0);
	Log("GUI | ImFont pointer = 0x%X", m_pFontGTAWeap);

	CRadarRect::LoadTextures();
}

CGUI::~CGUI()
{
    ImGui_ImplRenderWare_ShutDown();
    ImGui::DestroyContext();
}

ImFont* CGUI::LoadFont(char *font, float fontsize)
{
	ImGuiIO &io = ImGui::GetIO();

	// load fonts
	char path[0xFF];
	sprintf(path, "%sSAMP/fonts/%s", g_pszStorage, font);
	
	// ranges
	static const ImWchar ranges[] = 
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x04FF, // Russia
		0x0E00, 0x0E7F, // Thai
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
		0xF020, 0xF0FF, // Half-width characters
		0
	};
	
	ImFont* pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	return pFont;
}

void CGUI::RenderTextDeathMessage(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size, ImFont *font, bool bOutlineUseTextColor)
{
	int iOffset = bOutlineUseTextColor ? 1 : pSettings->Get().iFontOutline;
	if(bOutline)
	{
		// left
		posCur.x -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x += iOffset;
		// right
		posCur.x += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x -= iOffset;
		// above
		posCur.y -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y += iOffset;
		// below
		posCur.y += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y -= iOffset;
	}

	ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, col, text_begin, text_end);
}


void CGUI::SetupDefaultStyle() {

    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    style->ScrollbarSize = ScaleY(45.0f);
    colors[ImGuiCol_Text]                   = ImVec4(0.87f, 0.89f, 0.92f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.89f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 2.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.90f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.13f, 0.24f, 0.16f, 1.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.84f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.98f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.13f, 0.13f, 0.13f, 0.95f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.69f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.69f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.04f, 0.04f, 0.04f, 0.85f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.04f, 0.04f, 0.04f, 0.85f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.04f, 0.04f, 0.04f, 0.85f);
    colors[ImGuiCol_Separator]              = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.16f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

}

void CGUI::Render()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplRenderWare_NewFrame();
    ImGui::NewFrame();

    RenderVersion();
    RenderPosition();
    RenderFPS();
    pGameScreen->GetButtons()->ToggleState(true);

    if(pNetGame)
    {   
        pNetGame->GetPlayerBubblePool()->Draw();
        pNetGame->GetLabelPool()->Draw();
        pNetGame->GetTextDrawPool()->Draw();
    }

    if (pGameScreen) 
    {	
	pGameScreen->GetInterface()->RenderHud();	
                  pGameScreen->GetInterface()->RenderMenu();
	//if(!pSettings->Get().iNewButtons) 
                  pGButton->RenderButton();
	pFirstPersonCamera->Render();
    }
        
    if(pPlayerTags)     pPlayerTags->Render();
    if(pChatWindow)     pChatWindow->Render();
    if(pDialogWindow)   pDialogWindow->Render();
    if(pSpawnScreen)    pSpawnScreen->Render();
    if(pKeyBoard)       pKeyBoard->Render();
    if(pScoreBoard)     pScoreBoard->Render();//Draw();
    if(pDeathMessage)   pDeathMessage->Render();

/*    if (pNetGame)
	{
		if (pSettings->Get().szConnectIndicator)
		{
			//ImVec2 centre(ScaleX(35.0f), ScaleY(35.0f)); // слева
			ImVec2 centre(ScaleX(1880.0f), ScaleY(35.0f)); // справа
			if (pNetGame->GetGameState() == GAMESTATE_DISCONNECTED)
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(0.8f, 0.0f, 0.0f));
			}
			if (pNetGame->GetGameState() == GAMESTATE_CONNECTING || pNetGame->GetGameState() == GAMESTATE_WAIT_CONNECT)
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(1.0f, 1.0f, 0.0f));
			}
			if (pNetGame->GetGameState() == GAMESTATE_CONNECTED)
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(0.0f, 0.8f, 0.0f));
			}
		}
	}

    */
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

    if(m_bNeedClearMousePos)
    {
        io.MousePos = ImVec2(-1, -1);
        m_bNeedClearMousePos = false;
    }
}

bool CGUI::OnTouchEvent(int type, bool multi, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();

    if(pNetGame) {
        if (pChatWindow && !pChatWindow->OnTouchEvent(type, multi, x, y))
            return false;

        if (pKeyBoard && !pKeyBoard->OnTouchEvent(type, multi, x, y))
            return false;

        pNetGame->GetTextDrawPool()->OnTouchEvent(type, multi, x, y);
    }

    switch(type)
    {
        case TOUCH_PUSH:
            io.MousePos = ImVec2(x, y);
            io.MouseDown[0] = true;
            break;

        case TOUCH_POP:
            io.MouseDown[0] = false;
            m_bNeedClearMousePos = true;
            break;

        case TOUCH_MOVE:
            io.MousePos = ImVec2(x, y);

            ScrollDialog(x, y);

            m_iLastPosY = y;
            break;
    }

    return true;
}

void CGUI::RenderFPS()
{
	if(!pSettings->Get().iFPSCounter) return;
	CPatch::CallFunction<void>(g_libGTASA + 0x39A0C4 + 1);
	char buff[50];
	float count = CPatch::CallFunction<float>(g_libGTASA + 0x39A054 + 1);
	sprintf(buff, "%.1f", count);
	float PositionFPS;
	ImVec2 posCur;
	PositionFPS = m_vecScale.y * (float)(1080.0 - m_fFontSize);
    	posCur.x = m_vecScale.x * 20.0;
    	posCur.y = PositionFPS;
	RenderText(posCur, ImColor(255, 255, 255), 1, buff);
}

void CGUI::RenderVersion()
{
    ImGuiIO& io = ImGui::GetIO();
    char buffer[128];

    ImVec2 pos = ImVec2(10, 10);
    RenderText(pos, ImColor(255, 255, 255), true, "Placeit Mobile (by Weikton)", nullptr, 16);
}

void CGUI::RenderPosition()
{
    ImGuiIO& io = ImGui::GetIO();
    MATRIX4X4 matFromPlayer;

    CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
    pLocalPlayerPed->GetMatrix(&matFromPlayer);

    ImVec2 _ImVec2 = ImVec2(ScaleX(10), io.DisplaySize.y - ImGui::GetFontSize() * 0.85);

    char text[128];
    sprintf(text, "\t\tPosition > X: %.4f - Y: %.4f - Z: %.4f", matFromPlayer.pos.X, matFromPlayer.pos.Y, matFromPlayer.pos.Z);

    RenderText(_ImVec2, ImColor(255, 255, 255, 255), false, text, nullptr, ImGui::GetFontSize() * 0.85);
}

void CGUI::RenderRakNetStatistics()
{
         /* code */
}

void CGUI::SetupKeyboardStyle() {

    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    style->WindowBorderSize = 0.0f;
    style->ChildBorderSize  = 0.0f;
    style->PopupBorderSize  = 0.0f;
    style->FrameBorderSize  = 0.0f;

    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);

}

void CGUI::RenderText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size)
{
    int iOffset = pSettings->Get().iFontOutline;

    if (bOutline)
    {
        auto oAlpha = (col & 0xFF000000) >> 24;
        auto oColor = ImColor(IM_COL32(0, 0, 0, oAlpha));

        // left
        posCur.x -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(nullptr, font_size, posCur, oColor, text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(nullptr, font_size, posCur, oColor, text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(nullptr, font_size, posCur, oColor, text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(nullptr, font_size, posCur, oColor, text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetBackgroundDrawList()->AddText(nullptr, font_size, posCur, col, text_begin, text_end);
}

void CGUI::RenderOverlayText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
    int iOffset = pSettings->Get().iFontOutline;

    if(bOutline)
    {
        posCur.x -= iOffset;
        ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetOverlayDrawList()->AddText(posCur, col, text_begin, text_end);
}

void CGUI::ScrollDialog(float x, float y)
{
    if (m_imWindow != nullptr)
    {
        // --> Scroll Window --> --> --> --> -->
        if (m_iLastPosY > y)
            ImGui::SetWindowScrollY(m_imWindow, m_imWindow->Scroll.y + ImGui::GetFontSize() / 2);

        if (m_iLastPosY < y)
            ImGui::SetWindowScrollY(m_imWindow, m_imWindow->Scroll.y - ImGui::GetFontSize() / 2);
    }
}

void CGUI::RenderTextWithSize(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size)
{
    int iOffset = pSettings->Get().iFontOutline;

    if (bOutline)
    {
        // left
        posCur.x -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
}

void CGUI::SetHealth(float fhpcar){
   bHealth = static_cast<int>(fhpcar);
}

int CGUI::GetHealth(){
	return 1;//static_cast<int>(pVehicle->GetHealth());
}

void CGUI::SetDoor(int door){
	bDoor = door;
}

void CGUI::SetEngine(int engine){
	bEngine = engine;
}

void CGUI::SetLights(int lights){
	bLights = lights;
}

void CGUI::SetMeliage(float meliage){
	bMeliage = static_cast<int>(meliage);
}

void CGUI::SetEat(float eate){
	eat = static_cast<int>(eate);
}

int CGUI::GetEat(){
	return eat;
}

void CGUI::SetFuel(float fuel){
   m_fuel = static_cast<int>(fuel);
}

void CGUI::ShowSpeed(){
  if (!pGame || !pNetGame || !pGame->FindPlayerPed()->IsInVehicle()) {
	pJavaWrapper->HideSpeed();
	bMeliage =0;
	m_fuel = 0;
	return;
   }

   if (pGame->FindPlayerPed()->IsAPassenger()) {
	pJavaWrapper->HideSpeed();
	bMeliage =0;
	m_fuel = 0;
	return;
    }

    int i_speed = 0;
    bDoor =0;
    bEngine = 0;
    bLights = 0;
    float fHealth = 0;
    CVehicle *pVehicle = nullptr;
    CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
    CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
    VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
    pVehicle = pVehiclePool->GetAt(id);
    
    if(pPlayerPed)
    {
        if(pVehicle)
        {
            VECTOR vecMoveSpeed;
            pVehicle->GetMoveSpeedVector(&vecMoveSpeed);
            i_speed = sqrt((vecMoveSpeed.X * vecMoveSpeed.X) + (vecMoveSpeed.Y * vecMoveSpeed.Y) + (vecMoveSpeed.Z * vecMoveSpeed.Z)) * 180;
            bHealth = pVehicle->GetHealth();
            bDoor = pVehicle->GetDoorState();
            bEngine = pVehicle->GetEngineState();
            bLights = pVehicle->GetLightsState();
        }
    }
   if (pGame->FindPlayerPed()->IsInVehicle()) {
	pJavaWrapper->ShowSpeed();
	pJavaWrapper->UpdateSpeedInfo(i_speed, m_fuel, bHealth, bMeliage, bEngine, bLights, 0, bDoor);
	return;
   }
}