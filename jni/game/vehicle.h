#pragma once
#include "../game/object.h"

#define MAX_REPLACED_TEXTURES	32
#define MAX_REPLACED_TEXTURE_NAME	32

struct SReplacedTexture
{
	char szOld[MAX_REPLACED_TEXTURE_NAME];
	RwTexture* pTexture;
};

#pragma pack(1)
typedef struct _CAR_MOD_INFO
{
	uint8_t byteCarMod0;
	uint8_t byteCarMod1;
	uint8_t byteCarMod2;
	uint8_t byteCarMod3;
	uint8_t byteCarMod4;
	uint8_t byteCarMod5;
	uint8_t byteCarMod6;
	uint8_t byteCarMod7;
	uint8_t byteCarMod8;
	uint8_t byteCarMod9;
	uint8_t byteCarMod10;
	uint8_t byteCarMod11;
	uint8_t byteCarMod12;
	uint8_t byteCarMod13;
	uint8_t bytePaintJob;
	int iColor0;
	int iColor1;
} CAR_MOD_INFO;

class CVehicle : public CEntity
{
public:
	CVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation = 0.0f, bool bSiren = false);
	~CVehicle();

	void LinkToInterior(int iInterior);
	void SetColor(int iColor1, int iColor2);

	void SetHealth(float fHealth);
	float GetHealth();

	// 0.3.7
	bool IsOccupied();
	bool HasADriver();

	// 0.3.7
	void SetInvulnerable(bool bInv);
	bool IsInvulnerable();
	// 0.3.7
	bool IsDriverLocalPlayer();
	// 0.3.7
	bool HasSunk();

	void ProcessMarkers();

	void RemoveEveryoneFromVehicle();

	void SetWheelPopped(uint8_t bytePopped);
	void SetDoorState(int iState);

                  int GetDoorState();

	void SetNumberPlate(char *szPlate);

	void UpdateDamageStatus(uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage);

	unsigned int GetVehicleSubtype();

	void EnableEngine(bool bEnable);
	void SetEngineState(int iState);
	int GetEngineState();
	
	void EnableLights(bool bState);
	void SetLightsState(int iState);
	int GetLightsState();
	
	void SetHeadlightsColor(uint8_t r, uint8_t g, uint8_t b);
	void ProcessHeadlightsColor(uint8_t *r, uint8_t *g, uint8_t *b);

	bool GetSirenState();
	void SetSirenState(bool state);
	void SetAttachedObject(CObject *pObject);
	CObject* FindAttachedEntity(ENTITY_TYPE* pEntity);


    CVehicle* GetTrailer();
    void SetTrailer(CVehicle *pTrailer);
    void DetachTrailer();
    void AttachTrailer();
	
    bool IsATrailer();
    CVehicle* GetTractor();

    void ApplyTexture(const char* szTexture, const char* szNew);
    void ApplyTexture(const char* szTexture, RwTexture* pTexture);
    void RemoveTexture(const char* szOldTexture);
    bool IsRetextured(const char* szOldTexture);

    float GetTrainSpeed() const;
    void SetTrainSpeed(float fSpeed) const;
    bool IsATrainPart() const;

public:
	VEHICLE_TYPE	*m_pVehicle;
	bool 			m_bIsLocked;
	CVehicle		*m_pTrailer;
	uint32_t		m_dwMarkerID;
	bool 			m_bIsInvulnerable;
	bool 			m_bDoorsLocked;
	uint8_t			m_byteObjectiveVehicle; // Is this a special objective vehicle? 0/1
	uint8_t			m_bSpecialMarkerEnabled;

	bool			m_bIsEngineOn;
	bool 			m_bIsLightsOn;

	SReplacedTexture m_szReplacedTextures[MAX_REPLACED_TEXTURES];
	bool m_bReplaceTextureStatus[MAX_REPLACED_TEXTURES];
	bool m_bReplacedTexture;

	uint8_t			m_byteColor1;
	uint8_t			m_byteColor2;
	bool 			m_bColorChanged;

	// Attached objects
	std::list<CObject*> m_pAttachedObjects;

	char m_szTextNumberPlate[8];
	RwTexture* m_pPlateTexture;

                  int fDoorState;
	
	uint8_t			m_byteHeadLightColorsR;
	uint8_t			m_byteHeadLightColorsG;
	uint8_t			m_byteHeadLightColorsB;
	bool			m_bHeadLightsColorChanged;
};
