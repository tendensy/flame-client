#include "../main.h"
#include "../game/game.h"
#include "../game/RW/RenderWare.h"

extern CGame *pGame;

CSnapShotHelper::CSnapShotHelper()
{
	m_camera = 0;
	m_light = 0;
	m_frame = 0;
	m_zBuffer = 0;
	m_raster = 0;

	SetUpScene();
}

void CSnapShotHelper::SetUpScene()
{
	// RpLightCreate
	m_light = RpLightCreate(2);
	if (m_light == 0) return;

	RwRGBAReal color = {1.0f, 1.0f, 1.0f, 1.0f};
	RpLightSetColor(m_light, &color);
	m_camera = RwCameraCreate();
	m_frame = RwFrameCreate(); 
	RwV3d v = { 0.0f, 0.0f, 50.0f };
	RwFrameTranslate(m_frame, &v, rwCOMBINEPRECONCAT);
	v.x = 1.0f; v.y = 0.0f; v.z = 0.0f;
	RwFrameRotate(m_frame, &v, 90.0f, rwCOMBINEPRECONCAT);
	m_zBuffer = RwRasterCreate(256, 256, 0, rwRASTERTYPEZBUFFER);
	m_camera->zBuffer = m_zBuffer;

	RwObjectHasFrameSetFrame(m_camera, m_frame);
	RwCameraSetFarClipPlane(m_camera, 300.0f);
	RwCameraSetNearClipPlane(m_camera, 0.01f);

	RwV2d view = { 0.5f, 0.5f };
	RwCameraSetViewWindow(m_camera, view);
	RwCameraSetProjection(m_camera, rwPERSPECTIVE);

	// RpWorldAddCamera
	RpWorld* pRwWorld = util::game::GetWorldScene();
	if (pRwWorld) {
		RpWorldAddCamera(pRwWorld, m_camera);
	}
}

uintptr_t CSnapShotHelper::CreatePedSnapShot(int iModel, uint32_t dwColor, VECTOR* vecRot, float fZoom)
{
	Log("CreatePedSnapShot: %d, %f, %f, %f", iModel, vecRot->X, vecRot->Y, vecRot->Z);

	RwRaster* raster = RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	RwTexture* bufferTexture =  RwTextureCreate(raster);

	CPlayerPed *pPed = new CPlayerPed(208, 0, 0.0f, 0.0f, 0.0f, 0.0f);

	if(!raster || !bufferTexture || !pPed) return 0;

	float posZ = iModel == 162 ? 50.15f : 50.05f;
	float posY = fZoom * -2.25f;
	pPed->TeleportTo(0.0f, posY, posZ);
	pPed->SetModelIndex(iModel);
	pPed->SetGravityProcessing(false);
	pPed->SetCollisionChecking(false);

	MATRIX4X4 mat;
	pPed->GetMatrix(&mat);

	if(vecRot->X != 0.0f)
		RwMatrixRotate(&mat, 0, vecRot->X);
	if(vecRot->Y != 0.0f)
		RwMatrixRotate(&mat, 1, vecRot->Y);
	if(vecRot->Z != 0.0f)
		RwMatrixRotate(&mat, 2, vecRot->Z);

	pPed->UpdateMatrix(mat);

	// set camera frame buffer //
	m_camera->frameBuffer = raster;
	util::game::SetRenderWareCamera(m_camera);

	RwCameraClear(m_camera, (RwRGBA*)&dwColor, 3);
	RwCameraBeginUpdate(m_camera);

	// RpWorldAddLight
	RpWorld* pRwWorld = util::game::GetWorldScene();
	if (pRwWorld) {
		RpWorldAddLight(pRwWorld, m_light);
	}

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODENASHADEMODE);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

	util::game::DefinedState();

	pPed->Add();
	
	pPed->ClumpUpdateAnimations(100.0f, 1);
	pPed->Render();

	RwCameraEndUpdate(m_camera);

	if (pRwWorld) {
		RpWorldRemoveLight(pRwWorld, m_light);
	}

	pPed->Remove();

	delete pPed;
	Log("Done");

	return (uintptr_t)bufferTexture;
}

uintptr_t CSnapShotHelper::CreateVehicleSnapShot(int iModel, uint32_t dwColor, VECTOR* vecRot, float fZoom, int dwColor1, int dwColor2)
{
	Log("CreateVehicleSnapShot: %d %f, %f, %f, (%d, %d)", iModel, vecRot->X, vecRot->Y, vecRot->Z, dwColor1, dwColor2);

	RwRaster* raster = RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	RwTexture* bufferTexture = RwTextureCreate(raster);

	if(iModel == 570)
	{
		iModel = 538;
	}
	else if(iModel == 569)
	{
		iModel = 537;
	}

	CVehicle *pVehicle = new CVehicle(iModel, 0.0f, 0.0f, 50.0f, 0.0f);

	if(!raster || !bufferTexture || !pVehicle)
		return 0;

	pVehicle->SetGravityProcessing(false);
	pVehicle->SetCollisionChecking(false);

	float fRadius = GetModelColSphereRadius(iModel);
	float posY = (-1.0 - (fRadius + fRadius)) * fZoom;

	if(pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT)
	{
		posY = -5.5 - fRadius * 2.5;
	}

	pVehicle->TeleportTo(0.0f, posY, 50.0f);

	if(dwColor1 != 0xFFFF && dwColor2 != 0xFFFF)
		pVehicle->SetColor(dwColor1, dwColor2);

	MATRIX4X4 mat;
	pVehicle->GetMatrix(&mat);

	if(vecRot->X != 0.0f)
		RwMatrixRotate(&mat, 0, vecRot->X);
	if(vecRot->Y != 0.0f)
		RwMatrixRotate(&mat, 1, vecRot->Y);
	if(vecRot->Z != 0.0f)
		RwMatrixRotate(&mat, 2, vecRot->Z);

	pVehicle->UpdateMatrix(mat);

	m_camera->frameBuffer = raster;
	util::game::SetRenderWareCamera(m_camera);

	RwCameraClear(m_camera, (RwRGBA*)&dwColor, 3);
	RwCameraBeginUpdate(m_camera);

	RpWorld* pRwWorld = util::game::GetWorldScene();
	if (pRwWorld) {
		RpWorldAddLight(pRwWorld, m_light);
	}


	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODENASHADEMODE);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

	util::game::DefinedState();

	pVehicle->Add();
	pVehicle->Render();

	RwCameraEndUpdate((RwCamera*)m_camera);

	if (pRwWorld) {
		RpWorldRemoveLight(pRwWorld, m_light);
	}

	pVehicle->Remove();

	delete pVehicle;

	return (uintptr_t)bufferTexture;
}

uintptr_t CSnapShotHelper::CreateObjectSnapShot(int iModel, uint32_t dwColor, VECTOR* vecRot, float fZoom)
{
	Log("CreateObjectSnapShot: %d %f, %f, %f, (%d, %d)", iModel, vecRot->X, vecRot->Y, vecRot->Z);
	if (iModel == 1373 || iModel == 3118 || iModel == 3552 || iModel == 3553)
		iModel = 18631;

	bool bNeedRemoveModel = false;
	if (!pGame->IsModelLoaded(iModel))
	{
		pGame->RequestModel(iModel);
		pGame->LoadRequestedModels();
		while (!pGame->IsModelLoaded(iModel)) sleep(1);
		bNeedRemoveModel = true;
	}

	RwObject* pRwObject = (RwObject*)ModelInfoCreateInstance(iModel);

	float fRadius = GetModelColSphereRadius(iModel);

	VECTOR vecCenter = { 0.0f, 0.0f, 0.0f };
	GetModelColSphereVecCenter(iModel, &vecCenter);

	RwFrame* parent = (RwFrame*)pRwObject->parent;

	if (parent == 0) return 0;
	
	RwV3d v = {
		-vecCenter.X,
		(-0.1f - fRadius * 2.25f) * fZoom,
		50.0f - vecCenter.Z };
	RwFrameTranslate(parent, &v, rwCOMBINEPRECONCAT);

	if (iModel == 18631)
		{
			// RwFrameRotate X
			v.x = 0.0f;
			v.y = 0.0f;
			v.z = 1.0f;
			RwFrameRotate(parent, &v, 180.0f, rwCOMBINEPRECONCAT);
		}
	else
	{
		if (vecRot->X != 0.0f)
		{
			// RwFrameRotate X
			v.x = 1.0f;
			v.y = 0.0f;
			v.z = 0.0f;
			RwFrameRotate(parent, &v, vecRot->X, rwCOMBINEPRECONCAT);
		}

		if (vecRot->Y != 0.0f)
		{
			// RwFrameRotate Y
			v.x = 0.0f;
			v.y = 1.0f;
			v.z = 0.0f;
			RwFrameRotate(parent, &v, vecRot->Y, rwCOMBINEPRECONCAT);
		}

		if (vecRot->Z != 0.0f)
		{
			// RwFrameRotate Z
			v.x = 0.0f;
			v.y = 0.0f;
			v.z = 1.0f;
			RwFrameRotate(parent, &v, vecRot->Z, rwCOMBINEPRECONCAT);
		}
	}

	// RENDER DEFAULT //

	// set camera frame buffer //

	RwRaster* raster = RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	RwTexture* bufferTexture = RwTextureCreate(raster);
	m_camera->frameBuffer = raster;

	// CVisibilityPlugins::SetRenderWareCamera
	util::game::SetRenderWareCamera(m_camera);

	ProcessCamera((uintptr_t)pRwObject, dwColor);

	DestroyAtomicOrClump((uintptr_t)pRwObject);

	if (bNeedRemoveModel) {
		pGame->RemoveModel(iModel, false);
	}

	return (uintptr_t)bufferTexture;
}

void CSnapShotHelper::ProcessCamera(uintptr_t pRwObject, uint32_t dwColor)
{
	RwCameraClear(m_camera, (RwRGBA*)&dwColor, 3);
	RwCameraBeginUpdate((RwCamera*)m_camera);

	// RpWorldAddLight
	RpWorld* pRwWorld = util::game::GetWorldScene();
	if (pRwWorld) {
		RpWorldAddLight(pRwWorld, m_light);
	}

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODENASHADEMODE);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

	util::game::DefinedState();

	RenderClumpOrAtomic(pRwObject);

	RwCameraEndUpdate(m_camera);

	if (pRwWorld) {
		RpWorldRemoveLight(pRwWorld, m_light);
	}
}