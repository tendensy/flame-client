#pragma once

struct PlayerBubbleStruct
{
    char szText[256];
    float fDistance;
    uint32_t uiExpireTime;
    uint32_t uiColor;
};

class CPlayerBubblePool
{
public:
    CPlayerBubblePool();
    ~CPlayerBubblePool();

    PlayerBubbleStruct *New(PLAYERID playerId, const char *text, uint32_t color, float distance, uint32_t time);
    void Delete(PLAYERID playerId);

    void Draw();

private:
    PlayerBubbleStruct *m_pPlayerBubble[MAX_PLAYERS];
    bool m_bSlotState[MAX_PLAYERS];

};