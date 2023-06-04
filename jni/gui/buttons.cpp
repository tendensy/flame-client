#include "main.h"
#include "../game/game.h"
#include "net/netgame.h"
#include "dialog.h"
#include "GButton.h"
#include "scoreboard.h"
#include "keyboard.h"
#include "buttons.h"
#include "../settings.h"

extern CKeyBoard *pKeyBoard;
extern CDialogWindow *pDialogWindow;
extern CGame *pGame;
extern CScoreBoard *pScoreBoard;
extern CNetGame *pNetGame;
extern CGUI *pGUI;
extern CGButton *pGButton;
extern CSettings *pSettings;

CButtons::CButtons() {
    m_bIsItemShow = false;
    m_bIsShow = false;
}

void CButtons::Render() {
    if (pKeyBoard->IsOpen() || pDialogWindow->m_bIsActive) {
        return;
    }

    m_fButWidth = ImGui::CalcTextSize("/*QWERT*/QWERTY").x;
    m_fButHeight = ImGui::CalcTextSize("/*QWE*/QWERTY").x;
    ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * 3.5, ImGui::GetFontSize() * 2.5);

    CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
    if (pPlayerPed) {
        ImGuiIO& io = ImGui::GetIO();

	ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.5f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		ImGuiStyle style;
		style.FrameBorderSize = ImGui::GetStyle().FrameBorderSize;
		ImGui::GetStyle().FrameBorderSize = 1.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.5f, 6.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.96f, 0.56f, 0.19f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.96f, 0.56f, 0.19f, 1.0f));

        ImGui::Begin("#buttons#", nullptr, ImGuiWindowFlags_NoTitleBar /*| ImGuiWindowFlags_NoBackground */| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);

        if (m_bIsItemShow) {

	if (ImGui::Button("<<", vecButSize))
                m_bIsItemShow = !m_bIsItemShow;

	ImGui::SameLine(0, 5);

        if (ImGui::Button("ALT", vecButSize))
			{
				LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] = true;
				LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;
				pNetGame -> SendChatCommand("/pkt_fix_alt");
			}
			
			ImGui::SameLine(0, 5);
			if (ImGui::Button("INV", vecButSize))
			{
				pNetGame -> SendChatCommand("/inv");
			}
			ImGui::SameLine(0, 5);
			if (ImGui::Button("BIND", vecButSize))
			{
				pNetGame -> SendChatCommand("/binder");
			}
			
			ImGui::SameLine(0, 5);
	if (!pScoreBoard->GetState()) {
            if (ImGui::Button("TAB", vecButSize)) // TAB
                pScoreBoard->Toggle();
        }
        else {
            if (ImGui::Button("X", vecButSize))
                pScoreBoard->Toggle();

            //m_bIsItemShow = false;
        }

        ImGui::SameLine(0, 5);

            if (ImGui::Button("2", vecButSize))
                    LocalPlayerKeys.bKeys[ePadKeys::KEY_SUBMISSION] = true;

            ImGui::SameLine(0, 5);

            if (ImGui::Button("F", vecButSize))
                LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;

            ImGui::SameLine(0, 5);

            if (ImGui::Button("H", vecButSize))
                LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;

            ImGui::SameLine(0, 5);

            if (ImGui::Button("Y", vecButSize))
                LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;

            ImGui::SameLine(0, 5);

            if (ImGui::Button("N", vecButSize))
                LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;

            ImGui::SameLine(0, 5);
        }
        else {
		if (ImGui::Button(">>", vecButSize))
                m_bIsItemShow = !m_bIsItemShow;

		ImGui::SameLine(0, 5);
	
		if (pPlayerPed->IsInVehicle())
            {
               	if (ImGui::Button("Ctrl", vecButSize))
                LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION] = true;

            }
            else
	    {
            if (ImGui::Button("ALT", vecButSize))
			{
				LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] = true;
				LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;
				pNetGame -> SendChatCommand("/pkt_fix_alt");
				/*CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if (pPlayerPool)
			{
				CLocalPlayer* pLocalPlayer;
				if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
					LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] = true;
				else
					LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;
			}*/
			}
	    }
			//ImGui::SameLine(0, 5);
			/*ImGui::SameLine(0, 5);
			if (ImGui::Button("INV", vecButSize))
			{
				pNetGame -> SendChatCommand("/inv");
			}*/
			ImGui::SameLine(0, 5);
			/*if (ImGui::Button("AM", vecButSize))
			{
				pNetGame -> SendChatCommand("/actionmenu");
			}*/
			ImGui::SameLine(0, 5);
			if (ImGui::Button("BIND", vecButSize))
			{
				pNetGame -> SendChatCommand("/binder");
			}
            ImGui::SameLine(0, 5);
        }

        ImGui::SetWindowSize(ImVec2(-1, -1));
        //ImGui::SetWindowPos(ImVec2(2.0f, (io.DisplaySize.y / 3) - (m_fButWidth / 2) + io.DisplaySize.y / 35/*30*/));
	ImGui::SetWindowPos(ImVec2(2.0f, (io.DisplaySize.y / 3) - (vecButSize.x / 2) + io.DisplaySize.y / 35));
	//ImGui::SetNextWindowBgAlpha(0.1f);

        ImGui::End();
	ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(2);
	//ImGui::PopStyleVar();
       // ImGui::PopStyleColor(3);
    }
}

void CButtons::ToggleState(bool bShow)
{
   // m_bIsShow = !m_bIsShow;
      m_bIsShow = bShow;
}