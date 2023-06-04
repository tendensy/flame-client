#pragma once

// raknet
#include "../vendor/RakNet/RakClientInterface.h"
#include "../vendor/RakNet/RakNetworkFactory.h"
#include "../vendor/RakNet/PacketEnumerations.h"
#include "../vendor/RakNet/StringCompressor.h"

#include "../net/localplayer.h"
#include "../net/remoteplayer.h"
#include "../net/playerpool.h"
#include "../net/vehiclepool.h"
#include "../net/gangzonepool.h"
#include "../net/objectpool.h"
#include "../net/pickuppool.h"
#include "../net/textlabelpool.h"
#include "../net/textdrawpool.h"
#include "../net/playerbubblepool.h"
#include "../net/actorpool.h"

#define GAMESTATE_WAIT_CONNECT	9
#define GAMESTATE_CONNECTING	13
#define GAMESTATE_AWAIT_JOIN	15
#define GAMESTATE_CONNECTED 	14
#define GAMESTATE_RESTARTING	18
#define GAMESTATE_NONE 			0
#define GAMESTATE_DISCONNECTED	4

#define NETMODE_ONFOOT_SENDRATE	30
#define NETMODE_INCAR_SENDRATE	30
#define NETMODE_FIRING_SENDRATE 30
#define NETMODE_SEND_MULTIPLIER 2

#define INVALID_PLAYER_ID 0xFFFF
#define INVALID_VEHICLE_ID 0xFFFF

class CNetGame
{
public:
	CNetGame(const char* szHostOrIp, int iPort, const char* szPlayerName, const char* szPass);
	~CNetGame();

	void Process();

	CPlayerPool* GetPlayerPool() { return m_pPlayerPool; }
	CVehiclePool* GetVehiclePool() { return m_pVehiclePool; }
	CObjectPool* GetObjectPool() { return m_pObjectPool; }
	CPickupPool* GetPickupPool() { return m_pPickupPool; }
	CGangZonePool* GetGangZonePool() { return m_pGangZonePool; }
	CText3DLabelsPool* GetLabelPool() { return m_pLabelPool; }
	RakClientInterface* GetRakClient() { return m_pRakClient; };
	CTextDrawPool* GetTextDrawPool() { return m_pTextDrawPool; }
	CPlayerBubblePool* GetPlayerBubblePool() { return m_pPlayerBubblePool; }
	CActorPool* GetActorPool() { return m_pActorPool; }

	int GetGameState() { return m_iGameState; }
	void SetGameState(int iGameState) { m_iGameState = iGameState; }

	void ResetVehiclePool();
	void ResetObjectPool();
	void ResetPickupPool();
	void ResetGangZonePool();
	void ResetLabelPool();
	void ShutDownForGameRestart();

	void SendChatMessage(const char* szMsg);
	void SendChatCommand(const char* szMsg);
	void SendDialogResponse(uint16_t wDialogID, uint8_t byteButtonID, uint16_t wListboxItem, char* szInput);

	void SetMapIcon(uint8_t byteIndex, float fX, float fY, float fZ, uint8_t byteIcon, int iColor, int style);
	void DisableMapIcon(uint8_t byteIndex);

	void UpdatePlayerScoresAndPings();

	bool GetHeadMoveState();

private:
	RakClientInterface* m_pRakClient;
	CPlayerPool*		m_pPlayerPool;
	CVehiclePool*		m_pVehiclePool;
	CObjectPool*		m_pObjectPool;
	CPickupPool* 		m_pPickupPool;
	CGangZonePool*		m_pGangZonePool;
	CText3DLabelsPool*	m_pLabelPool;
	CTextDrawPool*		m_pTextDrawPool;
	CPlayerBubblePool*  m_pPlayerBubblePool;
	CActorPool*			m_pActorPool;
	int					m_iGameState;
	uint32_t			m_dwLastConnectAttempt;

	uint32_t			m_dwMapIcons[100];

	void UpdateNetwork();
	void Packet_AuthKey(Packet *p);
	void Packet_DisconnectionNotification(Packet *p);
	void Packet_ConnectionLost(Packet *p);
	void Packet_ConnectionSucceeded(Packet *p);
	void Packet_PlayerSync(Packet* pkt);
	void Packet_VehicleSync(Packet* pkt);
                  void Packet_TrailerSync(Packet *pkt);
	void Packet_PassengerSync(Packet* pkt);
	void Packet_MarkersSync(Packet* pkt);
	void Packet_AimSync(Packet* p);
	void Packet_BulletSync(Packet* pkt);
                  void Packet_CustomRPC(Packet* p);

	void verifyClient();

public:
	char m_szHostName[0xFF];
	char m_szHostOrIp[0x7F];
	int m_iPort;

	bool		m_bZoneNames;
	bool		m_bUseCJWalk;
	bool		m_bAllowWeapons;
	bool		m_bHeadMove;
	bool		m_bLimitGlobalChatRadius;
	float		m_fGlobalChatRadius;
	float 		m_fNameTagDrawDistance;
	bool		m_bDisableEnterExits;
	bool		m_bNameTagLOS;
	bool		m_bManualVehicleEngineAndLight;
	int 		m_iSpawnsAvailable;
	bool 		m_bShowPlayerTags;
	int 		m_iShowPlayerMarkers;
	bool		m_bHoldTime;
	uint8_t		m_byteWorldTime;
	uint8_t		m_byteWorldMinute;
	uint8_t		m_byteWeather;
	float 		m_fGravity;
	int 		m_iDeathDropMoney;
	bool 		m_bInstagib;
	int 		m_iLagCompensation;
	int 		m_iVehicleFriendlyFire;

	bool 		verify;
	bool		m_bDisableRemoteVehicleCollisions;
};