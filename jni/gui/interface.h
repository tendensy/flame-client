#pragma once

#include "../vendor/imgui/imgui.h"
#include "../game/RW/RenderWare.h"
#include "../game/common.h"

enum typeState
{
    OFF = 0,
    ON
};

class CInterface {

public:
    CInterface();
    ~CInterface();

    void RenderHud() const;
    void RenderMenu();
    //void RenderSpeedometer();
    //void RenderClientInfo();

    static void SetupKeyboardStyle();

    void ToggleHudState() { m_bRenderHud = !m_bRenderHud; }
    void ToggleMenuState() { m_bRenderMenu = !m_bRenderMenu; }
    //void ToggleInfoState() { m_bRenderInfo = !m_bRenderInfo; }

    void SetWantedLevel(int level) { m_iWantedLevel = level; }

    void SetHudID(int id) { m_iCurrentHudID = id; }
    int GetHudID() const { return m_iCurrentHudID; }

private:
    int m_iCurrentHudID;
    int m_iWantedLevel;
	
    int m_iCurrentVolumePlayer;

    bool m_bRenderHud;
    bool m_bRenderMenu;
    //bool m_bRenderInfo;

    RwTexture* hud_bg, *iHeart, *iEat, *iShield, *iWanted;
    RwTexture *iFist[47]; 

    //RECT m_rectArea;
};
