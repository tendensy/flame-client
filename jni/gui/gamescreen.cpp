#include "main.h"
#include "gamescreen.h"
#include "gui.h"

extern CGUI* pGUI;

CGameScreen::CGameScreen()
{
    m_pInterface    = new CInterface();
    m_pButtons      = new CButtons();
    //m_pNewButtons   = new CNewButtons();
}

void CGameScreen::LoadAllTextures()
{
   // panelBackground = LoadTex("speed_panel_backround");
   // panelEngine[0]  = LoadTex("speed_panel_engineOff"); panelEngine[1] = LoadTex("speed_panel_engineOn");
}

void CGameScreen::Render()
{
    //Speedometer();
}
