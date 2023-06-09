#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../chatwindow.h"
#include "../game/object.h"

extern CGame *pGame;
extern CChatWindow *pChatWindow;

CObjectPool::CObjectPool()
{
	for(uint16_t ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
	{
		m_bObjectSlotState[ObjectID] = false;
		m_pObjects[ObjectID] = 0;
	}
}

CObjectPool::~CObjectPool()
{
	for(uint16_t i = 0; i < MAX_OBJECTS; i++)
	{
		Delete(i);
	}
}

bool CObjectPool::Delete(uint16_t ObjectID)
{
	if(!GetSlotState(ObjectID) || !m_pObjects[ObjectID])
		return false;

	m_bObjectSlotState[ObjectID] = false;
	delete m_pObjects[ObjectID];
	m_pObjects[ObjectID] = 0;

	return true;
}

bool CObjectPool::New(uint16_t ObjectID, int iModel, VECTOR vecPos, VECTOR vecRot, float fDrawDistance)
{
	if(m_pObjects[ObjectID] != 0)
        Delete(ObjectID);

    m_pObjects[ObjectID] = pGame->NewObject(iModel, vecPos.X, vecPos.Y, vecPos.Z, vecRot, fDrawDistance);

    if(m_pObjects[ObjectID])
    {
        m_bObjectSlotState[ObjectID] = true;

        return true;
    }

    return false;
}

CObject *CObjectPool::GetObjectFromGtaPtr(ENTITY_TYPE *pGtaObject)
{
	uint16_t x=1;

	while(x!=MAX_OBJECTS)
	{
		if(m_pObjects[x])
			if(pGtaObject == m_pObjects[x]->m_pEntity) return m_pObjects[x];
		
		x++;
	}

	return 0;
}

uint16_t CObjectPool::FindIDFromGtaPtr(ENTITY_TYPE* pGtaObject)
{
	uint16_t x=1;

	while(x!=MAX_OBJECTS)
	{
		if(m_pObjects[x])
			if(pGtaObject == m_pObjects[x]->m_pEntity) return x;

		x++;
	}

	return INVALID_OBJECT_ID;
}

void CObjectPool::Process()
{
	static unsigned long s_ulongLastCall = 0;
	if (!s_ulongLastCall) s_ulongLastCall = GetTickCount();
	unsigned long ulongTick = GetTickCount();
	float fElapsedTime = ((float)(ulongTick - s_ulongLastCall)) / 1000.0f;
	// Get elapsed time in seconds
	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		if (m_bObjectSlotState[i]) m_pObjects[i]->Process(fElapsedTime);
	}
	s_ulongLastCall = ulongTick;
}

bool CObjectPool::ObjectCollisionProcess(int CColSphere1, int CColSphere2)
{
  	for(uint16_t i = 0; i < MAX_OBJECTS; i++)
	{
		if(m_pObjects[i] && m_bObjectSlotState[i])
		{
			if(m_pObjects[i]->IsAdded() &&
				m_pObjects[i]->GetDistanceFromCamera() <= 200.0f &&
				!m_pObjects[i]->IsEntityIgnored() &&
				m_pObjects[i]->TestSphereCastVsEntity(&CColSphere1, &CColSphere2, true))
			{
				return true;
			}
		}
	}

	return false;
}