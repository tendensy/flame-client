#pragma once

#include "../game/RW/RenderWare.h"
#include "../vendor/imgui/imgui.h"

class CButtons {

public:
    CButtons();

    void Render();
    void ToggleState(bool bShow);

private:
    float m_fButWidth;
    float m_fButHeight;

    bool m_bIsItemShow;
    bool m_bIsShow;

};
