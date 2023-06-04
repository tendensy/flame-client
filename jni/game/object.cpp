#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../chatwindow.h"
#include "materialtext.h"
#include "../util/patch.h"


extern CMaterialText *pMaterialText;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

CObject::CObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance)
{	
	if(!util::IsObjectInCdImage(iModel)) return;
	
	uint32_t dwRetID 	= 0;
	m_pEntity 			= 0;
	m_dwGTAId 			= 0;

	ScriptCommand(&create_object, iModel, fPosX, fPosY, fPosZ, &dwRetID);
	ScriptCommand(&put_object_at, dwRetID, fPosX, fPosY, fPosZ);

	m_vecRot = vecRot;
	m_vecTargetRotTar = vecRot;

	m_pEntity = GamePool_Object_GetAt(dwRetID);
	m_dwGTAId = dwRetID;
	m_byteMoving = 0;
	m_fMoveSpeed = 0.0;

	m_bIsPlayerSurfing = false;

	InstantRotate(vecRot.X, vecRot.Y, vecRot.Z);

	for (int i = 0; i < MAX_MATERIALS_PER_MODEL; i++)
	{
		m_MaterialTexture[i] = 0;
		m_dwMaterialColor[i] = new RwRGBA;
		*(uint32_t*)(m_dwMaterialColor[i]) = 0xFFFFFFFF;
	}
    m_bHasMaterial = false;
    for(int i = 0; i <= MAX_MATERIALS_PER_MODEL; i++)
    {
        m_MaterialTextTexture[i] = 0;
    }
    m_bHasMaterialText = false;

	m_fDrawDistance = fDrawDistance;
	m_bNeedRotate = false;
	m_fDistanceToTargetPoint = 0.0f;
}

CObject::~CObject()
{
    m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
    if(m_pEntity)
    {
        ScriptCommand(&destroy_object, m_dwGTAId);
    }
    for(int i = 0; i <= MAX_MATERIALS_PER_MODEL; i++)
    {
        m_MaterialTexture[i] = 0;
        m_dwMaterialColor[i] = 0;
    }

    m_bHasMaterial = false;

    for(int i = 0; i <= MAX_MATERIALS_PER_MODEL; i++)
    {
        m_MaterialTextTexture[i] = 0;
    }

    m_bHasMaterialText = false;
}

void CObject::UpdateRwMatrixAndFrame()
{
	if(m_pEntity && m_pEntity->vtable != g_libGTASA+0x5C7358)
	{
		if(m_pEntity->m_pRwObject)
		{
			if(m_pEntity->mat)
			{
				uintptr_t pRwMatrix = *(uintptr_t*)(m_pEntity->m_pRwObject + 4) + 0x10;

				if(!pRwMatrix) return;
				((void (*) (MATRIX4X4*, uintptr_t))(g_libGTASA+0x3E862C+1))(m_pEntity->mat, pRwMatrix);
				((void (*) (ENTITY_TYPE*))(g_libGTASA+0x39194C+1))(m_pEntity);
			}
		}
	}
}

void CObject::Process(float fElapsedTime)
{
	if(!m_bAttached && m_iAttachVehicle && m_iAttachVehicle != 65535)
	{
		CVehiclePool *vehiclePool = pNetGame->GetVehiclePool(); 
		CVehicle *vehicleObj = vehiclePool->GetAt(m_iAttachVehicle);
		if(vehicleObj)
		{
			if (m_vecAttachOffset.X > 10000.0f || m_vecAttachOffset.Y > 10000.0f || m_vecAttachOffset.Z > 10000.0f ||
				m_vecAttachOffset.X < -10000.0f || m_vecAttachOffset.Y < -10000.0f || m_vecAttachOffset.Z < -10000.0f)
			{ 
				// пропускаем действие
			}
			else
			{
				m_bAttached = true;
				ScriptCommand(&attach_object_to_car, m_dwGTAId, vehicleObj->m_dwGTAId, m_vecAttachOffset.X, m_vecAttachOffset.Y, m_vecAttachOffset.Z, m_vecAttachRot.X, m_vecAttachRot.Y, m_vecAttachRot.Z);
			}
		}
	}
	
	m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
	if (!m_pEntity) return;
	if (!(m_pEntity->mat)) return;
	if (m_byteMoving & 1)
	{
		MATRIX4X4 matPos;
		GetMatrix(&matPos);


		VECTOR matRot = m_vecRot;

		float distance = fElapsedTime * m_fMoveSpeed;
		float remaining = DistanceRemaining(&matPos, &m_matTarget);
		float remainingRot = RotaionRemaining(m_vecTargetRotTar, m_vecTargetRot);
		/*TeleportTo(m_matTarget.pos.X, m_matTarget.pos.Y, m_matTarget.pos.Z);
		InstantRotate(m_vecTargetRot.X, m_vecTargetRot.Y, m_vecTargetRot.Z);*/

		if (distance >= remaining)
		{
			m_byteMoving &= ~1; // Stop it moving
			// Force the final location so we don't overshoot slightly

			m_vecTargetRotTar = m_vecTargetRot;

			TeleportTo(m_matTarget.pos.X, m_matTarget.pos.Y, m_matTarget.pos.Z);
			InstantRotate(m_vecTargetRot.X, m_vecTargetRot.Y, m_vecTargetRot.Z);
		}
		else
		{
			// Else interpolate the new position between the current and final positions
			remaining /= distance; // Calculate ratio
			remainingRot /= distance;

			matPos.pos.X += (m_matTarget.pos.X - matPos.pos.X) / remaining;
			matPos.pos.Y += (m_matTarget.pos.Y - matPos.pos.Y) / remaining;
			matPos.pos.Z += (m_matTarget.pos.Z - matPos.pos.Z) / remaining;
			
			m_vecTargetRotTar.X += (m_vecTargetRot.X - m_vecTargetRotTar.X) / remaining;
			m_vecTargetRotTar.Y += (m_vecTargetRot.Y - m_vecTargetRotTar.Y) / remaining;
			m_vecTargetRotTar.Z += (m_vecTargetRot.Z - m_vecTargetRotTar.Z) / remaining;

			TeleportTo(matPos.pos.X, matPos.pos.Y, matPos.pos.Z);
			InstantRotate(m_vecTargetRotTar.X, m_vecTargetRotTar.Y, m_vecTargetRotTar.Z);
		}
	}

}

void CObject::GetRotation(float* pfX,float* pfY,float* pfZ)
{
    if (!m_pEntity) return;

	MATRIX4X4* mat = m_pEntity->mat;

	if(mat) CPatch::CallFunction<void>(g_libGTASA + 0x3E8098 + 1, mat, pfX, pfY, pfZ, 21);

	*pfX = *pfX * 57.295776 * -1.0;
	*pfY = *pfY * 57.295776 * -1.0;
	*pfZ = *pfZ * 57.295776 * -1.0;
    
}

float CObject::DistanceRemaining(MATRIX4X4 *matPos, MATRIX4X4 *m_matPositionTarget)
{
	float	fSX,fSY,fSZ;
	fSX = (matPos->pos.X - m_matPositionTarget->pos.X) * (matPos->pos.X - m_matPositionTarget->pos.X);
	fSY = (matPos->pos.Y - m_matPositionTarget->pos.Y) * (matPos->pos.Y - m_matPositionTarget->pos.Y);
	fSZ = (matPos->pos.Z - m_matPositionTarget->pos.Z) * (matPos->pos.Z - m_matPositionTarget->pos.Z);
	return (float)sqrt(fSX + fSY + fSZ);
}

float CObject::RotaionRemaining(VECTOR matPos, VECTOR m_vecRot)
{
	float fSX,fSY,fSZ;
	fSX = (matPos.X - m_vecRot.X) * (matPos.X - m_vecRot.X);
	fSY = (matPos.Y - m_vecRot.Y) * (matPos.Y - m_vecRot.Y);
	fSZ = (matPos.Z - m_vecRot.Z) * (matPos.Z - m_vecRot.Z);
	return (float)sqrt(fSX + fSY + fSZ);
}

void CObject::SetPos(float x, float y, float z)
{
	ScriptCommand(&put_object_at, m_dwGTAId, x, y, z);
}

void CObject::StopMoving()
{
	m_byteMoving = 0;
}

void CObject::MoveTo(float X, float Y, float Z, float speed, float rX, float rY, float rZ)
{
	m_matTarget.pos.X = X;
	m_matTarget.pos.Y = Y;
	m_matTarget.pos.Z = Z;

	m_vecTargetRot.X = rX;
	m_vecTargetRot.Y = rY;
	m_vecTargetRot.Z = rZ;
	
	m_fMoveSpeed = speed;
	m_byteMoving |= 1;
}

void CObject::MovePositionTo(float X, float Y, float Z)
{
	m_matTarget.pos.X = X;
	m_matTarget.pos.Y = Y;
	m_matTarget.pos.Z = Z;
	m_byteMoving |= 1;
}

void CObject::MoveRotationTo(float X, float Y, float Z)
{
	m_vecTargetRot.X = X;
	m_vecTargetRot.Y = Y;
	m_vecTargetRot.Z = Z;
	m_byteMoving |= 1;
}

void CObject::SetMovingSpeed(float speed)
{
	m_fMoveSpeed = speed;
}

void CObject::ApplyMoveSpeed()
{
	if(m_pEntity)
	{
		float fTimeStep	= *(float*)(g_libGTASA + 0x8C9BB4); // 2.00 - 0x96B500

		MATRIX4X4 mat;
		GetMatrix(&mat);
		mat.pos.X += fTimeStep * m_pEntity->vecMoveSpeed.X;
		mat.pos.Y += fTimeStep * m_pEntity->vecMoveSpeed.Y;
		mat.pos.Z += fTimeStep * m_pEntity->vecMoveSpeed.Z;
		UpdateMatrix(mat);
	}
}

void CObject::InstantRotate(float x, float y, float z)
{
	ScriptCommand(&set_object_rotation, m_dwGTAId, x, y, z);
}

void CObject::AttachProccess(uint16_t iAttachedVehicle, VECTOR AttachOffset, VECTOR AttachRot)
{
	m_iAttachVehicle = iAttachedVehicle;
	m_bAttached = false;
	m_vecAttachOffset = AttachOffset;
	m_vecAttachRot = AttachRot;
	
	/*if(!m_bAttached && m_iAttachVehicle && m_iAttachVehicle != 65535)
	{
		CVehiclePool *vehiclePool = pNetGame->GetVehiclePool();
		CVehicle *vehicleObj = vehiclePool->GetAt(m_iAttachVehicle);
		if(vehicleObj)
		{
			m_bAttached = true;
			ScriptCommand(&attach_object_to_car, m_dwGTAId, vehicleObj->m_dwGTAId, m_vecAttachOffset.X, m_vecAttachOffset.Y, m_vecAttachOffset.Z, m_vecAttachRot.X, m_vecAttachRot.Y, m_vecAttachRot.Z);
		}
	}*/
}

RwTexture* LoadFromTxdSlot(const char* szSlot, const char* szTexture, RwRGBA* rgba)
{	
	RwTexture* tex;
	if (strncmp(szSlot, "none", 5u))
	{	
		uintptr_t v10 = ((int (*)(const char*))(g_libGTASA + 0x55BB85))(szSlot);
		CPatch::CallFunction<void>(g_libGTASA + 0x55BD6C + 1);
   		CPatch::CallFunction<void>(g_libGTASA + 0x55BD6C + 1, v10, 0);
    	tex = CPatch::CallFunction<RwTexture*>(g_libGTASA + 0x1B2558 + 1, szTexture, 0);
    	CPatch::CallFunction<void>(g_libGTASA + 0x55BDA8 + 1);
	}

	if(!tex && strncmp(szTexture, "none", 5u))
	{
		if(!tex) tex = (RwTexture*)LoadTexture(std::string(std::string(szTexture) + "_" + szSlot).c_str());
		if(!tex) tex = (RwTexture*)LoadTexture(std::string(std::string(szSlot) + "_" + szTexture).c_str());
		if(!tex) tex = (RwTexture*)LoadTexture(szTexture);
		if(!tex)
		{
			std::string str = szTexture;
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
			tex = (RwTexture*)LoadTexture(str.c_str());
		}
		if(!tex)
		{
			std::string str = szTexture;
			std::transform(str.begin(), str.end(), str.begin(), ::toupper);
			tex = (RwTexture*)LoadTexture(std::string(str + "_" + szSlot).c_str());
		}
	}

	return tex;
}

void CObject::SetMaterial(int iModel, int iMaterialIndex, char* txdname, char* texturename, uint32_t dwColor)
{	
	
	if (iMaterialIndex < 16)
	{
		if (m_MaterialTexture[iMaterialIndex]) 
		{
			RwTextureDestroy(m_MaterialTexture[iMaterialIndex]);
			m_MaterialTexture[iMaterialIndex] = 0;
		}
		m_dwMaterialColor[iMaterialIndex] = (RwRGBA*)&dwColor;
		m_MaterialTexture[iMaterialIndex] = LoadFromTxdSlot(txdname, texturename, m_dwMaterialColor[iMaterialIndex]);
		m_bHasMaterial = true;
	}
}

void CObject::SetMaterialText(int iMaterialIndex, uint8_t byteMaterialSize, const char *szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char *szText)
{
    if (iMaterialIndex < 16)
	{
		if(m_MaterialTextTexture[iMaterialIndex])
    	{
        	RwTextureDestroy(m_MaterialTextTexture[iMaterialIndex]);
        	m_MaterialTextTexture[iMaterialIndex] = 0;
    	}
    	m_MaterialTextTexture[iMaterialIndex] = pMaterialText->Generate(byteMaterialSize, szFontName, byteFontSize, byteFontBold, dwFontColor, dwBackgroundColor, byteAlign, szText);
    	//m_dwMaterialTextColor[iMaterialIndex] = 0;
    	m_bHasMaterialText = true;
	}
}

void CObject::RotateMatrix(VECTOR vec)
{
	//ScriptCommand(&set_object_rotation, m_dwGTAId, vec.X, vec.Y, vec.Z);
	GetRotation(&vec.X, &vec.Y, &vec.Z);

	CPatch::CallFunction<void>(g_libGTASA + 0x3E8308 + 1, m_pEntity->mat, vec.X, vec.Y, vec.Z, 21);

}