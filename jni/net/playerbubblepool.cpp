#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "playerbubblepool.h"
#include "../gui/gui.h"
#include "../chatwindow.h"
#include "../game/playerped.h"

extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
extern CGUI *pGUI;

CPlayerBubblePool::CPlayerBubblePool()
{
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        this->m_pPlayerBubble[i] = 0;
        this->m_bSlotState[i] = false;
    }
}

CPlayerBubblePool::~CPlayerBubblePool()
{
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        if(this->m_pPlayerBubble[i] && this->m_bSlotState[i])
        {
            delete this->m_pPlayerBubble[i];
        }
        this->m_pPlayerBubble[i] = 0;
        this->m_bSlotState[i] = false;
    }
}

PlayerBubbleStruct *CPlayerBubblePool::New(PLAYERID playerId, const char *text, uint32_t color, float distance, uint32_t time)
{
    if(this->m_bSlotState[playerId] && this->m_pPlayerBubble[playerId])
    {
        delete this->m_pPlayerBubble[playerId];
    }

    this->m_pPlayerBubble[playerId] = new PlayerBubbleStruct;
    cp1251_to_utf8(m_pPlayerBubble[playerId]->szText, text);
    this->m_pPlayerBubble[playerId]->fDistance = distance;
    this->m_pPlayerBubble[playerId]->uiExpireTime = GetTickCount() + time;
    this->m_pPlayerBubble[playerId]->uiColor = color;
    this->m_bSlotState[playerId] = true;

    return this->m_pPlayerBubble[playerId];
}

void CPlayerBubblePool::Delete(PLAYERID playerId)
{
    if(this->m_bSlotState[playerId] && this->m_pPlayerBubble[playerId])
    {
        delete this->m_pPlayerBubble[playerId];
        this->m_bSlotState[playerId] = false;
    }
}
void Render3DLabel(ImVec2 pos, char* utf8string, uint32_t dwColor);
void CPlayerBubblePool::Draw()
{
    for(PLAYERID i = 0; i < MAX_PLAYERS; i++)
    {
        if(this->m_bSlotState[i] && this->m_pPlayerBubble[i])
        {   
            if(GetTickCount() >= m_pPlayerBubble[i]->uiExpireTime)
            {
                Delete(i);
                continue;
            }

            CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
            if(pPlayerPool->GetSlotState(i))
            {
                CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(i);
                if(pRemotePlayer && pRemotePlayer->IsActive())
                {
                    CPlayerPed *pPlayerPed = pRemotePlayer->GetPlayerPed();

                    if(pPlayerPed && pPlayerPed->IsAdded())
                    {
                        if(pPlayerPed->GetDistanceFromCamera() <= m_pPlayerBubble[i]->fDistance)
                        {
                            VECTOR VecPos;
                            VecPos.X = 0.0f;
                            VecPos.Y = 0.0f;
                            VecPos.Z = 0.0f;

                            pPlayerPed->GetBonePosition(8, &VecPos);

                            VECTOR TagPos;
                            TagPos.X = VecPos.X;
                            TagPos.Y = VecPos.Y;
                            TagPos.Z = VecPos.Z;

                            TagPos.Z += 0.4f + (pPlayerPed->GetDistanceFromCamera() * 0.0475f);

                            VECTOR Out;
                            // CSprite::CalcScreenCoors
                            (( void (*)(VECTOR*, VECTOR*, float*, float*, bool, bool))(g_libGTASA+0x54EEC0+1))(&TagPos, &Out, 0, 0, 0, 0);

                            if(Out.Z < 1.0f)
                                return;

                            ImVec2 pos = ImVec2(Out.X, Out.Y);

                            Render3DLabel(pos, m_pPlayerBubble[i]->szText, m_pPlayerBubble[i]->uiColor);
                        }
                    }
                }
            }
        }
    }
}