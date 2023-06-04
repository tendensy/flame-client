#pragma once

#include "RW/RenderWare.h"

class CObject : public CEntity
{
public:
	MATRIX4X4	m_matTarget;
	CQuaternion m_quatTarget;
	CQuaternion m_quatStart;
	MATRIX4X4	m_matCurrent;
	uint8_t		m_byteMoving;
	uint8_t		m_byteMovingRot;
	float		m_fMoveSpeed;
	bool		m_bIsPlayerSurfing;
	float		m_fDrawDistance;

	VECTOR 		m_vecRot;
	VECTOR		m_vecTargetRot;
	VECTOR		m_vecTargetRotTar;
	VECTOR      m_vPlayerSurfOffs;

	uint16_t 	m_iAttachVehicle;
	bool 		m_bAttached;
	VECTOR 		m_vecAttachOffset;
	VECTOR 		m_vecAttachRot;

	VECTOR 		m_vecRotationTarget;
	VECTOR		m_vecSubRotationTarget;

	uint32_t 	m_dwMoveTick;
	bool		m_bNeedRotate;
	float		m_fDistanceToTargetPoint;

	CObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance);
	~CObject();

	void UpdateRwMatrixAndFrame();

	void Process(float fElapsedTime);
	float DistanceRemaining(MATRIX4X4 *matPos, MATRIX4X4 *m_matPositionTarget);
	float RotaionRemaining(VECTOR matPos, VECTOR m_vecRot);
	void GetRotation(float* pfX,float* pfY,float* pfZ);
	void RotateMatrix(VECTOR vec);

	void SetPos(float x, float y, float z);
	void MoveTo(float x, float y, float z, float speed, float rX, float rY, float rZ);

	void MovePositionTo(float X, float Y, float Z);
	void MoveRotationTo(float X, float Y, float Z);
	void SetMovingSpeed(float speed);
	void ApplyMoveSpeed();
	void StopMoving();

	void InstantRotate(float x, float y, float z);

	void AttachProccess(uint16_t iAttachedVehicle, VECTOR AttachOffset, VECTOR AttachRot);
	void SetMaterial(int iModel, int iMaterialIndex, char* txdname, char* texturename, uint32_t dwColor);
    void SetMaterialText(int iMaterialIndex, uint8_t byteMaterialSize, const char *szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char *szText);

public:
	RwTexture*	m_MaterialTexture[MAX_MATERIALS_PER_MODEL];
	RwRGBA*		m_dwMaterialColor[MAX_MATERIALS_PER_MODEL];
	bool		m_bHasMaterial;

    RwTexture*	m_MaterialTextTexture[MAX_MATERIALS_PER_MODEL];
    RwRGBA*		m_dwMaterialTextColor[MAX_MATERIALS_PER_MODEL];
    bool		m_bHasMaterialText;
};