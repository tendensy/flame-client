#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../util/patch.h"
#include "../game/materialtext.h"
#include <algorithm>
#include "chatwindow.h"

// this file modifed from brilliant sources

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CMaterialText *pMaterialText;
extern CChatWindow *pChatWindow;
VEHICLE_TYPE *pLastVehicle;
uint8_t bInProcessDetachTrailer;

CVehicle::CVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation, bool bSiren)
{
	MATRIX4X4 mat;
	uint32_t dwRetID = 0;

	m_pVehicle = nullptr;
	m_dwGTAId = 0;
	m_pTrailer = nullptr;

	if( (iType != TRAIN_PASSENGER_LOCO) &&
		(iType != TRAIN_FREIGHT_LOCO) &&
		(iType != TRAIN_PASSENGER) &&
		(iType != TRAIN_FREIGHT) &&
		(iType != TRAIN_TRAM)) 
	{
		// normal vehicle
		if(!pGame->IsModelLoaded(iType))
		{
			pGame->RequestModel(iType);
			pGame->LoadRequestedModels(false);
			while(!pGame->IsModelLoaded(iType)) usleep(10);
		}

		ScriptCommand(&create_car, iType, fPosX, fPosY, fPosZ, &dwRetID);
		ScriptCommand(&set_car_z_angle, dwRetID, fRotation);
		ScriptCommand(&car_gas_tank_explosion,dwRetID, 0);
		ScriptCommand(&set_car_hydraulics, dwRetID, 0);
		ScriptCommand(&toggle_car_tires_vulnerable, dwRetID, 0);

		m_pVehicle = (VEHICLE_TYPE*)GamePool_Vehicle_GetAt(dwRetID);
		m_pEntity = (ENTITY_TYPE*)m_pVehicle;
		m_dwGTAId = dwRetID;

		if(m_pVehicle)
		{
			m_pVehicle->dwDoorsLocked = 0;
                                                      //m_bIsLocked = false;
			m_pVehicle->dwFlags.bEngineOn = 0;

			SetLightsState(0);

			m_pVehicle->dwFlags.bSirenOrAlarm = 0;

			GetMatrix(&mat);
			mat.pos.X = fPosX;
			mat.pos.Y = fPosY;
			mat.pos.Z = fPosZ;

			if( GetVehicleSubtype() != VEHICLE_SUBTYPE_BIKE && 
				GetVehicleSubtype() != VEHICLE_SUBTYPE_PUSHBIKE)
				mat.pos.Z += 0.25f;

			SetMatrix(mat);

			SetSirenState((int)bSiren);
		}
	}
	else if((iType == TRAIN_PASSENGER_LOCO) ||
			(iType == TRAIN_FREIGHT_LOCO) ||
			(iType == TRAIN_TRAM))
	{
        // train locomotives

        if(iType == TRAIN_PASSENGER_LOCO) iType = 5;
		else if(iType == TRAIN_FREIGHT_LOCO) iType = 3;
		else if(iType == TRAIN_TRAM) iType = 9;

		int dwDirection = 0;
		if(fRotation > 180.0f) dwDirection = 1;

		pGame->RequestModel(TRAIN_PASSENGER_LOCO);
		pGame->RequestModel(TRAIN_PASSENGER);
		pGame->RequestModel(TRAIN_FREIGHT_LOCO);
		pGame->RequestModel(TRAIN_FREIGHT);
		pGame->RequestModel(TRAIN_TRAM);
		pGame->LoadRequestedModels(false);
		while(!pGame->IsModelLoaded(TRAIN_PASSENGER_LOCO)) usleep(500);
		while(!pGame->IsModelLoaded(TRAIN_PASSENGER)) usleep(500);
		while(!pGame->IsModelLoaded(TRAIN_FREIGHT_LOCO)) usleep(500);
		while(!pGame->IsModelLoaded(TRAIN_FREIGHT)) usleep(500);
		while(!pGame->IsModelLoaded(TRAIN_TRAM)) usleep(500);
	
		ScriptCommand(&create_train, iType, fPosX, fPosY, fPosZ, dwDirection, &dwRetID);

		m_pVehicle = (VEHICLE_TYPE*)GamePool_Vehicle_GetAt(dwRetID);
		m_pEntity = (ENTITY_TYPE*)m_pVehicle;
		m_dwGTAId = dwRetID;
		pLastVehicle = m_pVehicle;
	}
	else if((iType == TRAIN_PASSENGER) ||
			iType == TRAIN_FREIGHT)
	{
                  if(!pLastVehicle) 
		{
			m_pEntity = 0;
			m_pVehicle = 0;
			pLastVehicle = 0;
			return;
		}

		m_pVehicle = (VEHICLE_TYPE *)pLastVehicle->VehicleAttachedBottom;

		if(!util::IsValidGameVehicle(m_pVehicle))
		{
			pChatWindow->AddDebugMessage("Warning: bad train carriages");
			m_pEntity = 0;
			m_pVehicle = 0;
			pLastVehicle = 0;
			return;
		}

		dwRetID = GamePool_Vehicle_GetIndex(m_pVehicle);
		m_pEntity = (ENTITY_TYPE *)m_pVehicle;
		m_dwGTAId = dwRetID;
		pLastVehicle = m_pVehicle;
	}

	m_byteObjectiveVehicle = 0;
	m_bSpecialMarkerEnabled = false;
	m_bDoorsLocked = false;
	m_dwMarkerID = 0;
	m_bIsInvulnerable = false;
	m_bIsEngineOn = true;
	m_bIsLightsOn = false;
	
	m_byteHeadLightColorsR = -1;
	m_byteHeadLightColorsG = -1;
	m_byteHeadLightColorsB = -1;
	m_bHeadLightsColorChanged = false;
	//strcpy(&m_szTextNumberPlate[0], "flame");
	//m_pPlateTexture = pMaterialText->GenerateNumberPlate("Flame RP");
}

CVehicle::~CVehicle()
{
	m_pVehicle = GamePool_Vehicle_GetAt(m_dwGTAId);

	if(m_pVehicle)
	{
		if(m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}

		RemoveEveryoneFromVehicle();

		if(GetSirenState()) 
			SetSirenState(0);

        if(m_pTrailer)
		{
			DetachTrailer();
			SetTrailer(NULL);
		}

        if(GetModelIndex() == TRAIN_PASSENGER_LOCO || GetModelIndex() == TRAIN_FREIGHT_LOCO)
			ScriptCommand(&destroy_train, m_dwGTAId);
		else
		{
			int nModelIndex = GetModelIndex();
			ScriptCommand(&destroy_car, m_dwGTAId);

			if(!GetModelReferenceCount(nModelIndex))
				pGame->RemoveModel(nModelIndex, true);
		}
    }
}

void CVehicle::LinkToInterior(int iInterior)
{
	if(GamePool_Vehicle_GetAt(m_dwGTAId)) 
	{
		ScriptCommand(&link_vehicle_to_interior, m_dwGTAId, iInterior);
	}
}


void CVehicle::SetColor(int iColor1, int iColor2)
{
	if(m_pVehicle)
	{
		if(GamePool_Vehicle_GetAt(m_dwGTAId))
		{
			m_pVehicle->byteColor1 = (uint8_t)iColor1;
			m_pVehicle->byteColor2 = (uint8_t)iColor2;
		}
	}

	m_byteColor1 = (uint8_t)iColor1;
	m_byteColor2 = (uint8_t)iColor2;
	m_bColorChanged = true;
}

void CVehicle::SetHealth(float fHealth)
{
	if(m_pVehicle)
	{
		m_pVehicle->fHealth = fHealth;
	}

	if(fHealth == 1000.0f) ScriptCommand(&repair_car, m_dwGTAId); // RPC RepairCar
}

float CVehicle::GetHealth()
{
	if(m_pVehicle) return m_pVehicle->fHealth;
	else return 0.0f;
}

CVehicle* CVehicle::GetTractor()
{
	if(util::IsValidGameVehicle(m_pVehicle))
	{
		uint32_t dwSubtype = GetVehicleSubtype();
		if(dwSubtype == VEHICLE_SUBTYPE_CAR || dwSubtype == VEHICLE_SUBTYPE_HELI || dwSubtype == VEHICLE_SUBTYPE_PLANE)
		{
			uint32_t dwTractorGTAPtr = m_pVehicle->dwTractor;
			if(pNetGame && dwTractorGTAPtr) 
			{
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
				if(pVehiclePool)
				{
					VEHICLEID tractorId = (VEHICLEID)pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE*)dwTractorGTAPtr);
					if(tractorId < MAX_VEHICLES && pVehiclePool->GetSlotState(tractorId))
						return pVehiclePool->GetAt(tractorId);
				}
			}
		}
	}

	return NULL;
}


// 0.3.7
void CVehicle::SetInvulnerable(bool bInv)
{
	if(!m_pVehicle) return;
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) return;
	if(m_pVehicle->entity.vtable == g_libGTASA+0x5C7358) return;

	if(bInv) 
	{
		ScriptCommand(&set_car_immunities, m_dwGTAId, 1,1,1,1,1);
		ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 0);
		m_bIsInvulnerable = true;
	} 
	else 
	{ 
		ScriptCommand(&set_car_immunities, m_dwGTAId, 0,0,0,0,0);
		ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 1);
		m_bIsInvulnerable = false;
	}
}

// Feature
bool CVehicle::IsInvulnerable()
{
	return m_bIsInvulnerable;
}

// 0.3.7
bool CVehicle::IsDriverLocalPlayer()
{
	if(m_pVehicle)
	{
		if((PED_TYPE*)m_pVehicle->pDriver == GamePool_FindPlayerPed())
			return true;
	}

	return false;
}

void CVehicle::ApplyTexture(const char* szTexture, const char* szNew)
{
	if (IsRetextured(szTexture))
	{
		RemoveTexture(szTexture);
	}

	uint8_t bID = 255;
	for (uint8_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
	{
		if (m_bReplaceTextureStatus[i] == false)
		{
			bID = i;
			break;
		}
	}

	if (bID == 255)
	{
		return;
	}

	m_bReplaceTextureStatus[bID] = true;
	strcpy(&(m_szReplacedTextures[bID].szOld[0]), szTexture);
	m_szReplacedTextures[bID].pTexture = (RwTexture*)LoadTextureFromDB("samp", szNew);

	m_bReplacedTexture = true;
}

void CVehicle::ApplyTexture(const char* szTexture, RwTexture* pTexture)
{
	if (IsRetextured(szTexture))
	{
		RemoveTexture(szTexture);
	}
	//pChatWindow->AddDebugMessage("apply tex %s", szTexture);
	uint8_t bID = 255;
	for (uint8_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
	{
		if (m_bReplaceTextureStatus[i] == false)
		{
			bID = i;
			break;
		}
	}

	if (bID == 255)
	{
		return;
	}

	m_bReplaceTextureStatus[bID] = true;
	strcpy(&(m_szReplacedTextures[bID].szOld[0]), szTexture);
	m_szReplacedTextures[bID].pTexture = pTexture;

	m_bReplacedTexture = true;
}

void CVehicle::RemoveTexture(const char* szOldTexture)
{
	for (size_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
	{
		if (m_bReplaceTextureStatus[i])
		{
			if (!strcmp(m_szReplacedTextures[i].szOld, szOldTexture))
			{
				m_bReplaceTextureStatus[i] = false;

				if (m_szReplacedTextures[i].pTexture)
				{
					RwTextureDestroy(m_szReplacedTextures[i].pTexture);
					m_szReplacedTextures[i].pTexture = nullptr;
				}
				break;
			}
		}
	}
}

bool CVehicle::IsRetextured(const char* szOldTexture)
{
	for (size_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
	{
		if (m_bReplaceTextureStatus[i])
		{
			if (!strcmp(m_szReplacedTextures[i].szOld, szOldTexture))
			{
				return true;
			}
		}
	}
	return false;
}

// 0.3.7
bool CVehicle::HasSunk()
{
	if(!m_pVehicle) return false;
	return ScriptCommand(&has_car_sunk, m_dwGTAId);
}

void CVehicle::RemoveEveryoneFromVehicle()
{
	if(!m_pVehicle) return;
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

	float fPosX = m_pVehicle->entity.mat->pos.X;
	float fPosY = m_pVehicle->entity.mat->pos.Y;
	float fPosZ = m_pVehicle->entity.mat->pos.Z;

	int iPlayerID = 0;
	if(m_pVehicle->pDriver)
	{
		iPlayerID = GamePool_Ped_GetIndex( m_pVehicle->pDriver );
		ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2.0f);
	}

	for(int i = 0; i<7; i++)
	{
		if(m_pVehicle->pPassengers[i] != nullptr)
		{
			iPlayerID = GamePool_Ped_GetIndex( m_pVehicle->pPassengers[i] );
			ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2.0f);
		}
	}
}

// 0.3.7
bool CVehicle::IsOccupied()
{
	if(m_pVehicle)
	{
		if(m_pVehicle->pDriver) return true;
		if(m_pVehicle->pPassengers[0]) return true;
		if(m_pVehicle->pPassengers[1]) return true;
		if(m_pVehicle->pPassengers[2]) return true;
		if(m_pVehicle->pPassengers[3]) return true;
		if(m_pVehicle->pPassengers[4]) return true;
		if(m_pVehicle->pPassengers[5]) return true;
		if(m_pVehicle->pPassengers[6]) return true;
	}

	return false;
}

bool CVehicle::HasADriver()
{
	if(!util::IsValidGameVehicle(m_pVehicle)) return false;
	
	if(m_pVehicle->pDriver && IN_VEHICLE(m_pVehicle->pDriver)) 
		return true;

	return false;
}

void CVehicle::ProcessMarkers()
{
	if(!m_pVehicle) return;

	if(m_byteObjectiveVehicle)
	{
		if(!m_bSpecialMarkerEnabled)
		{
			if(m_dwMarkerID)
			{
				ScriptCommand(&disable_marker, m_dwMarkerID);
				m_dwMarkerID = 0;
			}

			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 3, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1006);
			ScriptCommand(&show_on_radar, m_dwMarkerID, 3);
			m_bSpecialMarkerEnabled = true;
		}

		return;
	}

	if(m_byteObjectiveVehicle && m_bSpecialMarkerEnabled)
	{
		if(m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_bSpecialMarkerEnabled = false;
			m_dwMarkerID = 0;
		}
	}

	if(GetDistanceFromLocalPlayerPed() < 200.0f && !IsOccupied())
	{
		if(!m_dwMarkerID)
		{
			// show
			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 2, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1004);
		}
	}

	else if(IsOccupied() || GetDistanceFromLocalPlayerPed() >= 200.0f)
	{
		// remove
		if(m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}
	}
}


void CVehicle::SetNumberPlate(char *szPlate)
{
	if(!m_pVehicle) {
		return;
	}
	
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) {
		return;
	}
	
	memcpy(&m_szTextNumberPlate, szPlate, 8);
	m_pPlateTexture = pMaterialText->GenerateNumberPlate(szPlate);
}

void CVehicle::SetWheelPopped(uint8_t bytePopped)
{

}

void CVehicle::SetDoorState(int iState)
{
	if(!util::IsValidGameVehicle(m_pVehicle)) return;
                  // m_pVehicle->dwDoorsLocked = iState == 1 ? 2 : 0;
                  if (iState)
	{
		m_pVehicle->dwDoorsLocked = 2;
		m_bDoorsLocked = true;
		CVehicle::fDoorState = 1;
	}
	else
	{
		m_pVehicle->dwDoorsLocked = 0;
		m_bDoorsLocked = false;
		CVehicle::fDoorState = 0;
	}
}

int CVehicle::GetDoorState(){
	return CVehicle::fDoorState;
}

void CVehicle::EnableEngine(bool bEnable)
{
	if(!util::IsValidGameVehicle(m_pVehicle)) return;

	if(bEnable)
		m_pVehicle->byteFlags |= 0x10;
	else m_pVehicle->byteFlags &= 0xEF;
}

void CVehicle::SetEngineState(int iState)
{
	m_bIsEngineOn = iState;
}

int CVehicle::GetEngineState()
{
	return m_bIsEngineOn;
}

void CVehicle::EnableLights(bool bState)
{
	if(!util::IsValidGameVehicle(m_pVehicle)) return;

	if(bState)
	{
		m_pVehicle->byteMoreFlags &= 0xF7;
		m_pVehicle->byteFlags |= 0x40;
	}
	else
	{
		m_pVehicle->byteMoreFlags |= 8;
		m_pVehicle->byteFlags &= 0xBF;
	}
}

void CVehicle::SetLightsState(int iState)
{
	m_bIsLightsOn = iState;
}

int CVehicle::GetLightsState()
{
	return m_bIsLightsOn;
}

void CVehicle::SetHeadlightsColor(uint8_t r, uint8_t g, uint8_t b)
{
	if(util::IsValidGameVehicle(m_pVehicle))
	{
		m_byteHeadLightColorsR = r;
		m_byteHeadLightColorsG = g;
		m_byteHeadLightColorsB = b;
		m_bHeadLightsColorChanged = true;
	}
}

void CVehicle::ProcessHeadlightsColor(uint8_t *r, uint8_t *g, uint8_t *b)
{
	if(util::IsValidGameVehicle(m_pVehicle) && m_bHeadLightsColorChanged)
	{
		*r = m_byteHeadLightColorsR;
		*g = m_byteHeadLightColorsG;
		*b = m_byteHeadLightColorsB;
	}
}

void CVehicle::UpdateDamageStatus(uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage)
{

}

unsigned int CVehicle::GetVehicleSubtype()
{
	if(m_pVehicle)
	{
		if(m_pVehicle->entity.vtable == g_libGTASA+0x5CC9F0) // 0x871120
		{
			return VEHICLE_SUBTYPE_CAR;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCD48) // 0x8721A0
		{
			return VEHICLE_SUBTYPE_BOAT;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCB18) // 0x871360
		{
			return VEHICLE_SUBTYPE_BIKE;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CD0B0) // 0x871948
		{
			return VEHICLE_SUBTYPE_PLANE;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCE60) // 0x871680
		{
			return VEHICLE_SUBTYPE_HELI;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCC30) // 0x871528
		{
			return VEHICLE_SUBTYPE_PUSHBIKE;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CD428) // 0x872370
		{
			return VEHICLE_SUBTYPE_TRAIN;
		}
	}

	return 0;
}

void CVehicle::SetAttachedObject(CObject* pObject)
{
	if (std::find(m_pAttachedObjects.begin(), m_pAttachedObjects.end(), pObject) != m_pAttachedObjects.end()) return;
	m_pAttachedObjects.push_back(pObject);
}

CObject* CVehicle::FindAttachedEntity(ENTITY_TYPE* pEntity)
{
	for (auto& o : m_pAttachedObjects)
	{
		if (o->m_pEntity == pEntity) return o;
		continue;
	}
	return nullptr;
}

// ---> Trailer -- -- -- -- --
void CVehicle::AttachTrailer()
{
    if (m_pTrailer)
        ScriptCommand(&put_trailer_on_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);

    bInProcessDetachTrailer = 0;
}

void CVehicle::DetachTrailer()
{
    if (m_pTrailer)
        ScriptCommand(&detach_trailer_from_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);

    bInProcessDetachTrailer = 1;
}

void CVehicle::SetTrailer(CVehicle *pTrailer)
{
    m_pTrailer = pTrailer;
}

CVehicle* CVehicle::GetTrailer()
{
    if (!m_pVehicle) return nullptr;

    // Try to find associated trailer
    uint32_t dwTrailerGTAPtr = m_pVehicle->dwTrailer;

    if(pNetGame && dwTrailerGTAPtr)
    {
        CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
        auto TrailerID = (VEHICLEID)pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE*)dwTrailerGTAPtr);
        if(TrailerID < MAX_VEHICLES && pVehiclePool->GetSlotState(TrailerID))
        {
            return pVehiclePool->GetAt(TrailerID);
        }
    }

    return nullptr;
}

bool CVehicle::IsATrailer()
{
	if(!util::IsValidGameVehicle(m_pVehicle)) return false;

  	int nModel = GetModelIndex();
	return (nModel == 435 ||
		nModel == 450 || 
		nModel == 584 ||
		nModel == 591 || 
		nModel == 606 || 
		nModel == 607 || 
		nModel == 608 || 
		nModel == 610 || 
		nModel == 611
	);
}

// ---> Trailer -- -- -- -- --

// ---> Train -- -- -- -- --
float CVehicle::GetTrainSpeed() const
{
    return m_pVehicle->fTrainSpeed;
}

void CVehicle::SetTrainSpeed(float fSpeed) const
{
    m_pVehicle->fTrainSpeed = fSpeed;
}

bool CVehicle::IsATrainPart() const
{
    int nModel;

    if(m_pVehicle) {
        nModel = m_pVehicle->entity.nModelIndex;

        if(nModel == TRAIN_PASSENGER_LOCO) return true;
        if(nModel == TRAIN_PASSENGER) return true;
        if(nModel == TRAIN_FREIGHT_LOCO) return true;
        if(nModel == TRAIN_FREIGHT) return true;
        if(nModel == TRAIN_TRAM) return true;
    }

    return false;
}
// ---> Train -- -- -- -- --
const SCRIPT_COMMAND switch_car_siren					= { 0x0397, "ii" };
const SCRIPT_COMMAND is_car_siren_on					= { 0x0ABD, "i" };
void CVehicle::SetSirenState(bool state)
{
	//ScriptCommand(&switch_car_siren, m_dwGTAId, state == 1 ? 1 : 0);
	//m_pVehicle->dwFlags.bSirenOrAlarm = state == 1 ? true : false;
}

bool CVehicle::GetSirenState()
{
	return 0; //ScriptCommand(&is_car_siren_on, m_dwGTAId);
}