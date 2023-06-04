#include "main.h"
#include "scoreboard.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "vendor/imgui/imgui_internal.h"
#include "settings.h"

extern CNetGame *pNetGame;
extern CGame *pGame;
extern CGUI *pGUI;
extern CSettings *pSettings;

CScoreBoard::CScoreBoard()
{
	ImGuiIO &io = ImGui::GetIO();
	m_iOffset = 0;
    m_pPlayers = 0;
    m_playerCount = 0;
    m_Size = ImVec2(io.DisplaySize.x, io.DisplaySize.y);
	
    m_bSorted = false;
    m_bToggle = false;
	
	m_fSizeX = pGUI->ScaleX(1024.0f);
	m_fSizeY = pGUI->ScaleY(768.0f);
}

void CScoreBoard::Toggle(/*bool bToggle*/)
{
	m_bToggle = !m_bToggle;
	pGame->FindPlayerPed()->TogglePlayerControllable(!m_bToggle);
}

void SwapPlayerInfo(PLAYER_SCORE_INFO* psi1, PLAYER_SCORE_INFO* psi2)
{
	PLAYER_SCORE_INFO plrinf;
	memcpy(&plrinf, psi1, sizeof(PLAYER_SCORE_INFO));
	memcpy(psi1, psi2, sizeof(PLAYER_SCORE_INFO));
	memcpy(psi2, &plrinf, sizeof(PLAYER_SCORE_INFO));
}

void CScoreBoard::Render()
{
    	if(!m_bToggle) return;
	
	UpdatePlayerScoreInfo();
	pNetGame->UpdatePlayerScoresAndPings();
	
    PLAYERID endplayer = m_playerCount;
    char szPlayerId[11], szScore[11], szPing[11], szServerAddress[64];
    unsigned char RGBcolors[3];
	
	ImGuiIO &io = ImGui::GetIO();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.06f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.07f, 0.07f, 0.09f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.09f, 0.12f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.24f, 0.23f, 0.29f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.56f, 0.56f, 0.58f, 1.00f));
	
    ImGui::SetNextWindowSize(ImVec2(m_fSizeX, m_fSizeY), NULL);
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    ImGui::Begin("###beginScoreboard", NULL, /*ImGuiWindowFlags_NoScrollbar |*/ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

    ImGui::Columns(4, "###serverInfo", false);
	cp1251_to_utf8(szServerAddress, pNetGame->m_szHostName);
	ImGui::Text("%s", szServerAddress);
	ImGui::SetColumnWidth(-1, ((m_fSizeX - (ImGui::CalcTextSize("Players: 1000").x + pGUI->GetFontSize()))));
	ImGui::NextColumn();
	
	ImGui::Text("Players: %d", m_playerCount);
	ImGui::SetColumnWidth(-1, (m_fSizeX / 100 + pGUI->GetFontSize() + ImGui::CalcTextSize("Players: 1000").x));
	ImGui::NextColumn();
	
	ImGui::Columns(1);
	
	ImGui::Text("");
	
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::BeginChild("###headers", ImVec2(-1, pGUI->GetFontSize()+1.0f), false);
	
    ImGui::Columns(4, "###headersColumn", false);
	
    ImGui::Text("id");
	ImGui::SetColumnWidth(-1, m_fSizeX / 100 * 10);
	ImGui::NextColumn();
	
    ImGui::Text("name");
	ImGui::SetColumnWidth(-1, m_fSizeX / 100 * 60);
	ImGui::NextColumn();
	
    ImGui::Text("score");
	ImGui::SetColumnWidth(-1, m_fSizeX / 100 * 15);
	ImGui::NextColumn();
	
    ImGui::Text("ping");
	ImGui::SetColumnWidth(-1, m_fSizeX / 100 * 15);
	ImGui::NextColumn();
	
	ImGui::Columns(1);
	
	ImGui::EndChild();
	ImGui::PopStyleColor();
	
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.06f, 0.06f, 1.0f));
	
	ImGui::BeginChild("###list", ImVec2(-1, -1), true);
	
	ImGui::Columns(4, "###listColumn", false);
	
    for(uint32_t line = m_iOffset; line < endplayer; line++)
    {
        int tmpTabId = 0;
        ImVec2 differentOffsets;
        if(tmpTabId == 0)
        {
            ImGui::PushID(tmpTabId+line);

            std::stringstream ss;
            ss << line+tmpTabId;
            std::string s = ss.str();

            std::string itemid = "##" + s;
            bool is_selected = (m_iSelectedItem == line);

            if(ImGui::Selectable(itemid.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0.0f, pGUI->GetFontSize() + ImGui::GetStyle().ItemSpacing.y)))
            {
                if(m_iLastSelectedItem == m_iSelectedItem)
                {
                    RakNet::BitStream bsSend;
                    bsSend.Write(m_pPlayers[line].dwID);
                    bsSend.Write(0); // 0 from scoreboard - 1 from ? - 2 from ?
                    pNetGame->GetRakClient()->RPC(&RPC_ClickPlayer, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);

                    //if(m_bToggle) m_bToggle = false;
                    Toggle();
                }

                m_iLastSelectedItem = line;
            }

            if(ImGui::IsItemHovered())
                m_iSelectedItem = line;

            if(is_selected)
                ImGui::SetItemDefaultFocus();

            ImGui::SameLine();
        }
        sprintf(szPlayerId, "%d", m_pPlayers[line].dwID);
        sprintf(szScore, "%d", m_pPlayers[line].iScore);
        sprintf(szPing, "%d", m_pPlayers[line].dwPing);

        RGBcolors[0] = (m_pPlayers[line].dwColor - 0xFF000000) >> 16;
        RGBcolors[1] = ((m_pPlayers[line].dwColor - 0xFF000000) & 0x00FF00) >> 8;
        RGBcolors[2] = ((m_pPlayers[line].dwColor - 0xFF000000) & 0x0000FF);

        ImGui::TextColored(ImColor(RGBcolors[0], RGBcolors[1], RGBcolors[2]), "%s", szPlayerId);
		ImGui::SetColumnWidth(-1, m_fSizeX / 100 * 10);
        ImGui::NextColumn();

        ImGui::TextColored(ImColor(RGBcolors[0], RGBcolors[1], RGBcolors[2]), "%s", m_pPlayers[line].szName);
		ImGui::SetColumnWidth(-1, m_fSizeX / 100 * 60);
        ImGui::NextColumn();

        ImGui::TextColored(ImColor(RGBcolors[0], RGBcolors[1], RGBcolors[2]), "%s", szScore);
		ImGui::SetColumnWidth(-1, m_fSizeX / 100 * 15);
        ImGui::NextColumn();

        ImGui::TextColored(ImColor(RGBcolors[0], RGBcolors[1], RGBcolors[2]), "%s", szPing);
		ImGui::SetColumnWidth(-1, m_fSizeX / 100 * 15);
        ImGui::NextColumn();
    }
    ImGui::Columns(1);
    //ScrollWhenDraggingOnVoid();
    ImGui::EndChild();
	
    ImGui::End();
	
	ImGui::PopStyleVar();
	ImGui::PopStyleColor(/*6*/8/*2*/);
}

void CScoreBoard::UpdatePlayerScoreInfo()
{
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	PLAYERID playercount = pPlayerPool->GetCount() + 1;
	m_playerCount = playercount;

	if(m_iOffset > (playercount - 20)) m_iOffset = (playercount - 20);
	if(m_iOffset < 0) m_iOffset = 0;

	m_pPlayers = (PLAYER_SCORE_INFO*)malloc(playercount * sizeof(PLAYER_SCORE_INFO));
	memset(m_pPlayers, 0, playercount * sizeof(PLAYER_SCORE_INFO));
	m_pPlayers[0].dwID = pPlayerPool->GetLocalPlayerID();
	m_pPlayers[0].szName = pPlayerPool->GetLocalPlayerName();
	m_pPlayers[0].iScore = pPlayerPool->GetLocalPlayerScore();
	m_pPlayers[0].dwPing = pPlayerPool->GetLocalPlayerPing();
	m_pPlayers[0].dwColor = pPlayerPool->GetLocalPlayer()->GetPlayerColorAsARGB();
	PLAYERID i = 1, x;
	for(x = 0; x < MAX_PLAYERS; x++)
	{
		if (!pPlayerPool->GetSlotState(x)) continue;
		m_pPlayers[i].dwID = x;
		m_pPlayers[i].szName = pPlayerPool->GetPlayerName(x);
		m_pPlayers[i].iScore = pPlayerPool->GetRemotePlayerScore(x);
		m_pPlayers[i].dwPing = pPlayerPool->GetRemotePlayerPing(x);
		m_pPlayers[i].dwColor = pPlayerPool->GetAt(x)->GetPlayerColorAsARGB();
		m_pPlayers[i].iState = (int)pPlayerPool->GetAt(x)->GetState();

		i++;
	}

	if(m_bSorted)
	{
		for (i = 0; i < playercount - 1; i++)
		{
			for (PLAYERID j = 0; j < playercount - 1 - i; j++)
			{
				if (m_pPlayers[j + 1].iScore > m_pPlayers[j].iScore)
				{
					SwapPlayerInfo(&m_pPlayers[j], &m_pPlayers[j + 1]);
                }
            }
        }
	}
}
