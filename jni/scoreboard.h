#pragma once

#define PLAYER_INFO_TICK_UPDATE_RATE 1000

typedef struct _PLAYER_SCORE_INFO
{
    uint32_t dwID;
    char *szName;
    int iScore;
    int32_t dwPing;
    uint32_t dwColor;
    int iState;
} PLAYER_SCORE_INFO;

class CScoreBoard
{
public:
    CScoreBoard();
    ~CScoreBoard();
	
    void Toggle(/*bool bToggle*/);

    void Process();
    void Render();
	
	bool GetState() { return m_bToggle; };
	
private:
	void UpdatePlayerScoreInfo();
	
private:
	float m_fSizeX;
	float m_fSizeY;
	ImVec2 m_Size;

    int m_iOffset;
    uint16_t m_playerCount;
    PLAYER_SCORE_INFO *m_pPlayers;
	
    bool m_bSorted;
    bool m_bToggle;
    
    int                 m_iLastSelectedItem;
    int                 m_iSelectedItem;
};
