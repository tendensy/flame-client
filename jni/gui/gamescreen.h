#pragma once
#include "buttons.h"
#include "interface.h"
#include "../GButton.h"
#include "../game/game.h"

class CGameScreen
{
private:
    CButtons* m_pButtons;
//    CNewButtons* m_pNewButtons;
    CInterface* m_pInterface;
public:
    CGameScreen(/* args */);
    ~CGameScreen();

    CButtons* GetButtons() { return m_pButtons; }
   // CNewButtons* GetNewButtons() { return m_pNewButtons; }
    CInterface* GetInterface() { return m_pInterface; }

    void Render();

private:
    RwTexture* LoadTex(const char* name) { return (RwTexture*)LoadTextureFromDB("samp", name ); }
    void LoadAllTextures();
  //  void Speedometer();
//    RwTexture* panelBackground, *panelEngine[2];
};
