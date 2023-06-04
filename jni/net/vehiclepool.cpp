#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../chatwindow.h"
#include "../game/util.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

CVehiclePool::CVehiclePool()
{
	for(VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++)
	{
		m_bVehicleSlotState[VehicleID] = false;
		m_pVehicles[VehicleID] = nullptr;
		m_pGTAVehicles[VehicleID] = nullptr;
	}
}

CVehiclePool::~CVehiclePool()
{
	for(VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++)
		Delete(VehicleID);
}

void CVehiclePool::Process()
{
	CVehicle *pVehicle;
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();

	for(VEHICLEID x = 0; x < MAX_VEHICLES; x++)
	{
		if(GetSlotState(x))
		{
			pVehicle = m_pVehicles[x];

			if(m_bIsActive[x])
			{
				if(pVehicle->IsDriverLocalPlayer()) pVehicle->SetInvulnerable(false);
				else pVehicle->SetInvulnerable(true);

				if(pVehicle->GetHealth() == 0.0f)
				{
						NotifyVehicleDeath(x);
						continue;
				}

				if(pVehicle->GetVehicleSubtype() != VEHICLE_SUBTYPE_BOAT &&
					pVehicle->GetDistanceFromLocalPlayerPed() < 200.0f &&
					pVehicle->HasSunk())
				{
					NotifyVehicleDeath(x);
					continue;
				}

				if(pNetGame->m_bManualVehicleEngineAndLight)
				{
					pVehicle->EnableEngine(pVehicle->GetEngineState() == 1);
					pVehicle->EnableLights(pVehicle->GetLightsState() == 1);
				}
				else
				{
					if(pVehicle->GetEngineState() == -1)
					{
						if(!pVehicle->HasADriver())
							pVehicle->EnableEngine(false);
						else
							pVehicle->EnableEngine(true);
					}
					else if(pVehicle->GetEngineState() != -1)
					{
						if(pVehicle->GetEngineState() == 0)
							pVehicle->EnableEngine(false);
						if(pVehicle->GetEngineState() == 1)
							pVehicle->EnableEngine(true);
                 	}

					pVehicle->EnableLights(pVehicle->GetLightsState() == 1);
				}

				if(pVehicle->m_pVehicle != m_pGTAVehicles[x])
					m_pGTAVehicles[x] = pVehicle->m_pVehicle;

				pVehicle->ProcessMarkers();
			}
		}
	}
}

#include "../game/customplate.h"
bool CVehiclePool::New(NEW_VEHICLE *pNewVehicle)
{
	if(!IsValidVehicleId(pNewVehicle->VehicleID)) {
		return false;
	}

	if(m_pVehicles[pNewVehicle->VehicleID] != nullptr)
	{
		pChatWindow->AddDebugMessage("Warning: vehicle %u was not deleted", pNewVehicle->VehicleID);
		Delete(pNewVehicle->VehicleID);
	}

	m_pVehicles[pNewVehicle->VehicleID] = pGame->NewVehicle(pNewVehicle->iVehicleType,
		pNewVehicle->vecPos.X, pNewVehicle->vecPos.Y, pNewVehicle->vecPos.Z, 
		pNewVehicle->fRotation, pNewVehicle->byteAddSiren);

	int random = rand() % 10;
	switch (random)
	{
		case 0:
		{
			CCustomPlateManager::PushPlate(pNewVehicle->VehicleID, 1, "c089mk", "28");
			break;
		}
		case 1:
		{
			CCustomPlateManager::PushPlate(pNewVehicle->VehicleID, 1, "c089mk", "28");
			break;
		}
		case 2:
		{
			CCustomPlateManager::PushPlate(pNewVehicle->VehicleID, 1, "c089mk", "28");
			break;
		}
		case 3:
		{
			CCustomPlateManager::PushPlate(pNewVehicle->VehicleID, 1, "c089mk", "28");
			break;
		}
		case 4:
		{
			CCustomPlateManager::PushPlate(pNewVehicle->VehicleID, 1, "c089mk", "28");
			break;
		}
		case 5:
		{
			CCustomPlateManager::PushPlate(pNewVehicle->VehicleID, 1, "c089mk", "28");
			break;
		}
		case 6:
		{
                                                      CCustomPlateManager::PushPlate(pNewVehicle->VehicleID, 1, "c089mk", "28");
			break;
		}
		case 7:
		{
			CCustomPlateManager::PushPlate(pNewVehicle->VehicleID, 1, "c089mk", "28");
			break;
		}
		case 8:
		{
			CCustomPlateManager::PushPlate(pNewVehicle->VehicleID, 1, "c089mk", "28");
			break;
		}
		case 9:
		{
			CCustomPlateManager::PushPlate(pNewVehicle->VehicleID, 1, "c089mk", "28");
			break;
		}
		case 10:
		{
			CCustomPlateManager::PushPlate(pNewVehicle->VehicleID, 1, "c089mk", "28");
			break;
		}
	}

	if(m_pVehicles[pNewVehicle->VehicleID])
	{
		// colors
		if(pNewVehicle->aColor1 != -1 || pNewVehicle->aColor2 != -1)
		{
			m_pVehicles[pNewVehicle->VehicleID]->SetColor(
				pNewVehicle->aColor1, pNewVehicle->aColor2 );
		}

		// health
		m_pVehicles[pNewVehicle->VehicleID]->SetHealth(pNewVehicle->fHealth);

		// gta handle
		m_pGTAVehicles[pNewVehicle->VehicleID] = m_pVehicles[pNewVehicle->VehicleID]->m_pVehicle;
		m_bVehicleSlotState[pNewVehicle->VehicleID] = true;

		// interior
		if(pNewVehicle->byteInterior > 0)
			LinkToInterior(pNewVehicle->VehicleID, pNewVehicle->byteInterior);

		// damage status
		if(pNewVehicle->dwPanelDamageStatus || 
			pNewVehicle->dwDoorDamageStatus || 
			pNewVehicle->byteLightDamageStatus)
		{
			m_pVehicles[pNewVehicle->VehicleID]->UpdateDamageStatus(
				pNewVehicle->dwPanelDamageStatus, 
				pNewVehicle->dwDoorDamageStatus,
				pNewVehicle->byteLightDamageStatus);
		}

		m_pVehicles[pNewVehicle->VehicleID]->SetWheelPopped(pNewVehicle->byteTireDamageStatus);

		m_bIsActive[pNewVehicle->VehicleID] = true;
		m_bIsWasted[pNewVehicle->VehicleID] = false;

		return true;
	}

	return false;
}

bool CVehiclePool::Delete(VEHICLEID VehicleID)
{
    Log("CVehiclePool::Delete -> removing vID: %d", VehicleID);

	if(!IsValidVehicleId(VehicleID)) {
		return false;
	}

	if(!GetSlotState(VehicleID) || !m_pVehicles[VehicleID])
		return false;

	m_bVehicleSlotState[VehicleID] = false;
	delete m_pVehicles[VehicleID];
	m_pVehicles[VehicleID] = nullptr;
	m_pGTAVehicles[VehicleID] = nullptr;

    Log("CVehiclePool::Delete -> vID: %d successfully removed", VehicleID);

	return true;
}

VEHICLEID CVehiclePool::FindIDFromGtaPtr(VEHICLE_TYPE *pGtaVehicle)
{
	int x=1;

	while(x != MAX_VEHICLES) 
	{
		if(pGtaVehicle == m_pGTAVehicles[x]) return x;
		x++;
	}

	return INVALID_VEHICLE_ID;
}

int CVehiclePool::FindGtaIDFromID(int iID)
{
	if(!IsValidVehicleId(iID)) {
		return 0;
	}

	if(m_pGTAVehicles[iID])
		return GamePool_Vehicle_GetIndex(m_pGTAVehicles[iID]);
	else
		return INVALID_VEHICLE_ID;
}

int CVehiclePool::FindNearestToLocalPlayerPed()
{
	float fLeastDistance = 10000.0f;
	float fThisDistance = 0.0f;
	VEHICLEID ClosetSoFar = INVALID_VEHICLE_ID;

	VEHICLEID x = 0;
	while(x < MAX_VEHICLES)
	{
		if(GetSlotState(x) && m_bIsActive[x])
		{
			fThisDistance = m_pVehicles[x]->GetDistanceFromLocalPlayerPed();
			if(fThisDistance < fLeastDistance)
			{
				fLeastDistance = fThisDistance;
				ClosetSoFar = x;
			}
		}

		x++;
	}

	return ClosetSoFar;
}

void CVehiclePool::LinkToInterior(VEHICLEID VehicleID, int iInterior)
{
	if(!IsValidVehicleId(VehicleID)) {
		return;
	}

	if(m_bVehicleSlotState[VehicleID])
		m_pVehicles[VehicleID]->LinkToInterior(iInterior);
}

void CVehiclePool::NotifyVehicleDeath(VEHICLEID VehicleID)
{
	if(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_LastVehicle != VehicleID) return;
	Log("CVehiclePool::NotifyVehicleDeath");

	RakNet::BitStream bsDeath;
	bsDeath.Write(VehicleID);
	pNetGame->GetRakClient()->RPC(&RPC_VehicleDestroyed, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	pNetGame->GetPlayerPool()->GetLocalPlayer()->m_LastVehicle = INVALID_VEHICLE_ID;
}

void CVehiclePool::AssignSpecialParamsToVehicle(VEHICLEID VehicleID, uint8_t byteObjective, uint8_t byteDoorsLocked)
{
	if(!GetSlotState(VehicleID)) return;
	CVehicle *pVehicle = m_pVehicles[VehicleID];

	if(pVehicle && m_bIsActive[VehicleID])
	{
		if(byteObjective)
		{
			pVehicle->m_byteObjectiveVehicle = 1;
			pVehicle->m_bSpecialMarkerEnabled = false;
		}

		pVehicle->SetDoorState(byteDoorsLocked);
	}
}

bool CVehiclePool::VehicleCollisionProcess(int CColSphere1, int CColSphere2)
{
  	for(uint16_t i = 0; i < MAX_VEHICLES; i++)
	{
		if(m_pVehicles[i] && GetSlotState(i))
		{
			if(m_pVehicles[i]->IsAdded() &&
				m_pVehicles[i]->GetDistanceFromCamera() <= 300.0f &&
				!m_pVehicles[i]->IsEntityIgnored() &&
				m_pVehicles[i]->TestSphereCastVsEntity(&CColSphere1, &CColSphere2, true))
			{
				return true;
			}
		}
	}

	return false;
}