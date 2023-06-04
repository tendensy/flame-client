#include "../main.h"
#include "game.h"
#include "../util/patch.h"
#include "../net/netgame.h"
#include "txdstore.h"
#define PI 3.14159265
extern CGame *pGame;
extern CNetGame* pNetGame;

util::sWidgets util::pWidgets;
bool util::radar::bSquare = false;

uintptr_t dwPlayerPedPtrs[PLAYER_PED_SLOTS];

extern char *PLAYERS_REALLOC;

PED_TYPE *GamePool_FindPlayerPed()
{
	return *(PED_TYPE **) PLAYERS_REALLOC;
}

PED_TYPE *GamePool_Ped_GetAt(int iID)
{
	return ((PED_TYPE * (*)(int))(g_libGTASA + 0x41DD7C + 1)) (iID);
}

int GamePool_Ped_GetIndex(PED_TYPE * pActor)
{
	return ((int (*)(PED_TYPE *))(g_libGTASA + 0x41DD60 + 1)) (pActor);
}

VEHICLE_TYPE *GamePool_Vehicle_GetAt(int iID)
{
	return ((VEHICLE_TYPE * (*)(int))(g_libGTASA + 0x41DD44 + 1)) (iID);
}

int GamePool_Vehicle_GetIndex(VEHICLE_TYPE * pVehicle)
{
	return ((int (*)(VEHICLE_TYPE *))(g_libGTASA + 0x41DD28 + 1)) (pVehicle);
}

ENTITY_TYPE *GamePool_Object_GetAt(int iID)
{
	ENTITY_TYPE *(*GetPoolObj) (int iID);
	*(void **)(&GetPoolObj) = (void *)(g_libGTASA + 0x41DDB4 + 1);
	return (GetPoolObj) (iID);
}

int FireSniper(WEAPON_SLOT_TYPE* pWeaponSlot, PED_TYPE* pPed)
{
	// CWeapon::FireSniper
	return ((int (*)(WEAPON_SLOT_TYPE*, PED_TYPE*))(g_libGTASA+0x56668C+1))(pWeaponSlot, pPed);
}

int GetWeaponSkill(PED_TYPE* pPed)
{
	// CWeaponInfo::GetWeaponSkill
	return ((int (*)(PED_TYPE*))(g_libGTASA+0x434F24+1))(pPed);
}

int LineOfSight(VECTOR * start, VECTOR * end, void *colpoint, uintptr_t ent,
				char buildings, char vehicles, char peds, char objects, char dummies,
				bool seeThrough, bool camera, bool unk)
{
	return ((int (*)(VECTOR *, VECTOR *, void *, uintptr_t,
					 char, char, char, char, char, char, char,
					 char))(g_libGTASA + 0x3C70C0 + 1)) (start, end, colpoint, ent, buildings,
														 vehicles, peds, objects, dummies,
														 seeThrough, camera, unk);
}

// 0.3.7
bool IsPedModel(unsigned int iModelID)
{
	if (iModelID < 0 || iModelID > 20000)
		return false;
	uintptr_t *dwModelArray = (uintptr_t *) (g_libGTASA + 0x87BF48);

	uintptr_t ModelInfo = dwModelArray[iModelID];
	if (ModelInfo && *(uintptr_t *) ModelInfo == (uintptr_t) (g_libGTASA + 0x5C6E90	/* CPedModelInfo 
																					   vtable */ ))
		return true;

	return false;
}

// 0.3.7
bool IsValidModel(unsigned int uiModelID)
{
	if (uiModelID < 0 || uiModelID > 20000)
		return false;
	uintptr_t *dwModelArray = (uintptr_t *) (g_libGTASA + 0x87BF48);

	uintptr_t dwModelInfo = dwModelArray[uiModelID];
	if (dwModelInfo && *(uintptr_t *) (dwModelInfo + 0x34 /* pRwObject */ ))
		return true;

	return false;
}

uint16_t GetModelReferenceCount(int nModelIndex)
{
	uintptr_t *dwModelarray = (uintptr_t *) (g_libGTASA + 0x87BF48);
	uint8_t *pModelInfoStart = (uint8_t *) dwModelarray[nModelIndex];

	return *(uint16_t *) (pModelInfoStart + 0x1E);
}

void InitPlayerPedPtrRecords()
{
	memset(&dwPlayerPedPtrs[0], 0, sizeof(uintptr_t) * PLAYER_PED_SLOTS);
}

void SetPlayerPedPtrRecord(uint8_t bytePlayer, uintptr_t dwPedPtr)
{
	if (bytePlayer >= PLAYER_PED_SLOTS)
		return;
	dwPlayerPedPtrs[bytePlayer] = dwPedPtr;
}

uint8_t FindPlayerNumFromPedPtr(uintptr_t dwPedPtr)
{
	uint8_t x = 0;
	while (x != PLAYER_PED_SLOTS)
	{
		if (dwPlayerPedPtrs[x] == dwPedPtr)
			return x;
		x++;
	}

	return 0;
}

bool IsPointInRect(float x, float y, RECT * rect)
{
	if (x >= rect->fLeft && x <= rect->fRight && y >= rect->fBottom && y <= rect->fTop)
		return true;

	return false;
}

uintptr_t LoadTextureFromDB(const char *dbname, const char *texture)
{
	// TextureDatabaseRuntime::GetDatabase(dbname)
	uintptr_t db_handle = ((uintptr_t(*)(const char *))(g_libGTASA + 0x1BF530 + 1)) (dbname);
	if (!db_handle)
	{
		//Log("Error: Database not found! (%s)", dbname);
		return 0;
	}
	// TextureDatabaseRuntime::Register(db)
	((void (*)(uintptr_t))(g_libGTASA + 0x1BE898 + 1)) (db_handle);
	uintptr_t tex = GetTexture(texture);

	if (!tex) {
		Log("Error: Texture (%s) not found in database (%s)", texture, dbname);
		return 0;
	}

	// TextureDatabaseRuntime::Unregister(db)
	((void (*)(uintptr_t))(g_libGTASA + 0x1BE938 + 1)) (db_handle);

	return tex;
}



void SetScissorRect(void *pRect)
{
	return ((void (*)(void *))(g_libGTASA + 0x273E8C + 1)) (pRect);
}

float DegToRad(float fDegrees)
{
	if (fDegrees > 360.0f || fDegrees < 0.0f)
		return 0.0f;

	if (fDegrees > 180.0f)
		return (float)(-(PI - (((fDegrees - 180.0f) * PI) / 180.0f)));
	else
		return (float)((fDegrees * PI) / 180.0f);
}

// 0.3.7
float FloatOffset(float f1, float f2)
{
	if (f1 >= f2)
		return f1 - f2;
	else
		return (f2 - f1);
}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../vendor/imgui/stb_image_write.h"

struct RwRaster* GetRWRasterFromBitmapPalette(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	if (!pBitmap)
	{
		return nullptr;
	}
	int len;
	unsigned char* png = stbi_write_png_to_mem(pBitmap, dwStride, dwX, dwY, 1, &len);
	if (!png)
	{
		return nullptr;
	}

	CPatch::WriteMemory(g_libGTASA + 0x1D6F9A, (uintptr_t)"\x03\x20", 2); // rwSTREAMMEMORY

	RwMemory memoryImage;
	RwInt32 width, height, depth, flags;

	memoryImage.start = (RwUInt8*)png;
	memoryImage.length = (RwUInt32)len;

	RwImage* pImage = RtPNGImageRead((const RwChar*)& memoryImage);

	if (a)
	{
		pImage->palette->alpha = a;
	}
	if (r)
	{
		pImage->palette->red = r;
	}
	if (g)
	{
		pImage->palette->green = g;
	}
	if (b)
	{
		pImage->palette->blue = b;
	}

	if (!pImage)
	{
		STBIW_FREE(png);
		return nullptr;
	}

	RwImageFindRasterFormat(pImage, 4, &width, &height, &depth, &flags);

	RwRaster* pRaster = RwRasterCreate(width, height, depth, flags);

	if (!pRaster)
	{
		STBIW_FREE(png);
		RwImageDestroy(pImage);
		return nullptr;
	}

	RwRasterSetFromImage(pRaster, pImage);

	RwImageDestroy(pImage);
	STBIW_FREE(png);

	CPatch::WriteMemory(g_libGTASA + 0x1D6F9A, (uintptr_t)"\x02\x20", 2); // rwSTREAMFILENAME

	return pRaster;
}

struct RwRaster* GetRWRasterFromBitmap(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY)
{
	if (!pBitmap)
	{
		return nullptr;
	}
	int len;
	unsigned char* png = stbi_write_png_to_mem(pBitmap, dwStride, dwX, dwY, 1, &len);
	if (!png)
	{
		return nullptr;
	}

	CPatch::WriteMemory(g_libGTASA + 0x1D6F9A, (uintptr_t)"\x03\x20", 2); // rwSTREAMMEMORY

	RwMemory memoryImage;
	RwInt32 width, height, depth, flags;

	memoryImage.start = (RwUInt8*)png;
	memoryImage.length = (RwUInt32)len;

	RwImage* pImage = RtPNGImageRead((const RwChar*)& memoryImage);

	if (!pImage)
	{
		STBIW_FREE(png);
		return nullptr;
	}

	RwImageFindRasterFormat(pImage, 4, &width, &height, &depth, &flags);

	RwRaster* pRaster = RwRasterCreate(width, height, depth, flags);

	if (!pRaster)
	{
		STBIW_FREE(png);
		RwImageDestroy(pImage);
		return nullptr;
	}

	RwRasterSetFromImage(pRaster, pImage);

	RwImageDestroy(pImage);
	STBIW_FREE(png);

	CPatch::WriteMemory(g_libGTASA + 0x1D6F9A, (uintptr_t)"\x02\x20", 2); // rwSTREAMFILENAME

	return pRaster;
}

void ProjectMatrix(VECTOR * vecOut, MATRIX4X4 * mat, VECTOR * vecPos)
{
	vecOut->X =
		mat->at.X * vecPos->Z + mat->up.X * vecPos->Y + mat->right.X * vecPos->X + mat->pos.X;
	vecOut->Y =
		mat->at.Y * vecPos->Z + mat->up.Y * vecPos->Y + mat->right.Y * vecPos->X + mat->pos.Y;
	vecOut->Z =
		mat->at.Z * vecPos->Z + mat->up.Z * vecPos->Y + mat->right.Z * vecPos->X + mat->pos.Z;
}

void RwMatrixRotate(MATRIX4X4 * mat, int axis, float angle)
{
	static float matt[3][3] = {
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	};

	((void (*)(MATRIX4X4 *, float *, float, int))(g_libGTASA + 0x1B9118 + 1)) (mat, matt[axis],
																			   angle, 1);
}

void RwMatrixScale(MATRIX4X4 * matrix, VECTOR * vecScale)
{
	matrix->right.X *= vecScale->X;
	matrix->right.Y *= vecScale->X;
	matrix->right.Z *= vecScale->X;

	matrix->up.X *= vecScale->Y;
	matrix->up.Y *= vecScale->Y;
	matrix->up.Z *= vecScale->Y;

	matrix->at.X *= vecScale->Z;
	matrix->at.Y *= vecScale->Z;
	matrix->at.Z *= vecScale->Z;

	matrix->flags &= 0xFFFDFFFC;
}

void WorldAddEntity(uintptr_t pEnt)
{
	((void (*)(uintptr_t))(g_libGTASA + 0x3C14B0 + 1)) (pEnt);
}

void WorldRemoveEntity(uintptr_t pEnt)
{
	((void (*)(uintptr_t))(g_libGTASA + 0x3C1500 + 1)) (pEnt);
}

uintptr_t GetModelInfoByID(int iModelID)
{
	if (iModelID < 0 || iModelID > 20000)
	{
		return false;
	}

	uintptr_t *dwModelArray = (uintptr_t *) (g_libGTASA + 0x87BF48);
	return dwModelArray[iModelID];
}


uintptr_t ModelInfoCreateInstance(int iModel)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);
	if (modelInfo)
	{
		return ((uintptr_t(*)(uintptr_t)) *
				(uintptr_t *) (*(uintptr_t *) modelInfo + 0x2C)) (modelInfo);
	}

	return 0;
}

void RenderClumpOrAtomic(uintptr_t rwObject)
{
	if (rwObject)
	{
		if (*(uint8_t *) rwObject == 1)
		{
			// Atomic
			((void (*)(uintptr_t))(*(uintptr_t *) (rwObject + 0x48))) (rwObject);
		}
		else if (*(uint8_t *) rwObject == 2)
		{
			// rpClumpRender
			((void (*)(uintptr_t))(g_libGTASA + 0x1E0E60 + 1)) (rwObject);
		}
	}
}


float GetModelColSphereRadius(int iModel)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);

	if (modelInfo)
	{
		uintptr_t colModel = *(uintptr_t *) (modelInfo + 0x2C);
		if (colModel != 0)
		{
			return *(float *)(colModel + 0x24);
		}
	}

	return 0.0f;
}


void GetModelColSphereVecCenter(int iModel, VECTOR * vec)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);

	if (modelInfo)
	{
		uintptr_t colModel = *(uintptr_t *) (modelInfo + 0x2C);
		if (colModel != 0)
		{
			VECTOR *v = (VECTOR *) (colModel + 0x18);

			vec->X = v->X;
			vec->Y = v->Y;
			vec->Z = v->Z;
		}
	}
}

void DestroyAtomicOrClump(uintptr_t rwObject)
{
	if (rwObject)
	{
		int type = *(int *)(rwObject);

		if (type == 1)
		{
			// RpAtomicDestroy
			((void (*)(uintptr_t))(g_libGTASA + 0x1E10D4 + 1)) (rwObject);

			uintptr_t parent = *(uintptr_t *) (rwObject + 4);
			if (parent)
			{
				// RwFrameDestroy
				((void (*)(uintptr_t))(g_libGTASA + 0x1AEC84 + 1)) (parent);
			}

		}
		else if (type == 2)
		{
			// RpClumpDestroy
			((void (*)(uintptr_t))(g_libGTASA + 0x1E1224 + 1)) (rwObject);
		}
	}
}

bool bTextDrawTextureSlotState[200];
uintptr_t TextDrawTexture[200];

int GetFreeTextDrawTextureSlot()
{
	for (int i = 0; i < 200; i++)
	{
		if (bTextDrawTextureSlotState[i] == false)
		{
			bTextDrawTextureSlotState[i] = true;
			return i;
		}
	}

	return -1;
}

void DestroyTextDrawTexture(int index)
{
	if (index >= 0 && index < 200)
	{
		bTextDrawTextureSlotState[index] = false;
		//RwTextureDestroy((RwTexture*)TextDrawTexture[index]);
		TextDrawTexture[index] = 0x0;
	}
}

uintptr_t LoadTexture(const char *texname)
{
	static char *texdb[] = { "samp", "mobile", "playerhi", "txd", "menu", "gta3", "gta_int", "player"};

	std::string str = texname;
	//std::transform(str.begin(), str.end(), str.begin(), ::tolower);

	for (int i = 0; i < 9; i++)
	{
		uintptr_t texture = LoadTextureFromDB(texdb[i], str.c_str());
		if (texture != 0)
		{
			Log("%s loaded from %s", str.c_str(), texdb[i]);
			return texture;
		}
		else continue;
	}

	//Log("Texture %s not found!", str.c_str());
	return 0;
}

uintptr_t LoadTextureFromTxd(const char* database, const char *texture)
{
	uintptr_t pRwTexture = 0;
	int g_iTxdSlot = ((int (__fastcall *)(const char *))(g_libGTASA + 0x55BB85))(database); // CTxdStore::FindTxdSlot
	if(g_iTxdSlot == -1)
	{
		Log("INVALID TXD: %s", database);
		uintptr_t db_handle = (( uintptr_t (*)(const char*))(g_libGTASA+0x1BF530+1))("samp");
		if(!db_handle) return 0;

		// TextureDatabaseRuntime::Register(db)
		(( void (*)(uintptr_t))(g_libGTASA+0x1BE898+1))(db_handle);
		uintptr_t tex = GetTexture(texture);
		// TextureDatabaseRuntime::Unregister(db)
		(( void (*)(uintptr_t))(g_libGTASA+0x1BE938+1))(db_handle);

		if(!tex) Log("Error: Texture %s not found in database %s", texture, database);

		return tex;
	}
	else
	{
		((void (*)(void))(g_libGTASA + 0x55BD6D))(); // CTxdStore::PushCurrentTxd
		((void (__fastcall *)(int, uint32_t))(g_libGTASA + 0x55BCDD))(g_iTxdSlot, 0);
		((void (__fastcall *)(uintptr_t *, const char *))(g_libGTASA + 0x551855))(&pRwTexture, texture); // CSprite2d::SetTexture

		if(!pRwTexture) {
			Log("INVALID TEXTURE | TXD: %s TEXTURE: %s", database, texture);
            uintptr_t db_handle = (( uintptr_t (*)(const char*))(g_libGTASA+0x1BF530+1))("samp");
			if(!db_handle) return 0;

			// TextureDatabaseRuntime::Register(db)
			(( void (*)(uintptr_t))(g_libGTASA+0x1BE898+1))(db_handle);
			uintptr_t tex = GetTexture(texture);
			// TextureDatabaseRuntime::Unregister(db)
			(( void (*)(uintptr_t))(g_libGTASA+0x1BE938+1))(db_handle);

			if(!tex) Log("Error: Texture %s not found in database %s", texture, database);

			return tex;
		}

		((void (*)(void))(g_libGTASA + 0x55BDA9))(); // CTxdStore::PopCurrentTxd
	}

	return pRwTexture;
}

int Game_PedStatPrim(int model_id)
{
	int *pStat;
	uint32_t *d = (uint32_t *)(g_libGTASA+0x87BF48+(model_id*4));
	pStat = (int *)((*d) + 40);
	return *pStat;	
}

void DrawTextureUV(uintptr_t texture, RECT * rect, uint32_t dwColor, float *uv)
{
	if (texture)
	{
		RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERLINEAR);
		// CSprite2d::Draw(CRect const& posn, CRGBA const& color, float u1,
		// float v1, float u2, float v2, float u3, float v3, float u4, float
		// v4);
		((void (*)
		  (uintptr_t, RECT *, uint32_t *, float, float, float, float, float, float, float,
		   float))(g_libGTASA + 0x5526CC + 1)) (texture, rect, &dwColor, uv[0], uv[1], uv[2],
												uv[3], uv[4], uv[5], uv[6], uv[7]);
	}
}

bool IsValidGamePed(PED_TYPE * pPed)
{
	// IsPedPointerValid(CPed *) — 0x00435614
	if (((bool(*)(PED_TYPE *)) (g_libGTASA + 0x00435614 + 1)) (pPed))
	{
		return true;
	}
	return false;
}

uintptr_t LoadFromTxdSlot(const char* szSlot, const char* szTexture)
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

	return (uintptr_t)tex;
}

bool IsValidGameEntity(ENTITY_TYPE * pEntity)
{
	// IsEntityPointerValid(CEntity *) — 0x00393870
	if (((bool(*)(ENTITY_TYPE *)) (g_libGTASA + 0x00393870 + 1)) (pEntity))
	{
		return true;
	}
	return false;
}

bool IsGameEntityArePlaceable(ENTITY_TYPE * pEntity)
{
	if (pEntity)
	{
		if (pEntity->vtable == g_libGTASA + 0x005C7358)
		{
			return true;
		}
	}
	return false;
}

bool PreloadAnimFile(char *szAnimFile, uint16_t usSleep, uint16_t usHowTimes)
{
	if (pGame->IsAnimationLoaded(szAnimFile))
	{
		return true;
	}

	pGame->RequestAnimation(szAnimFile);

	int iWaitAnimLoad = 0;
	while (!pGame->IsAnimationLoaded(szAnimFile))
	{
		usleep(usSleep);
		if (usHowTimes != -1)
		{
			if (++iWaitAnimLoad > usHowTimes)
			{
				return false;
			}
		}
	}
	return true;
}

uintptr_t FindRwTexture(std::string szTexDb, std::string szTexName) {
	uintptr_t pRwTexture = GetTexture(szTexName + std::string("_") + szTexDb);
	if (!pRwTexture) {
		pRwTexture = GetTexture(szTexDb + std::string("_") + szTexName);
		if (!pRwTexture) {
			pRwTexture = GetTexture(szTexName);
			if (!pRwTexture) {
				return 0;
			}
		}
	}
	return pRwTexture;
}


uintptr_t GetTexture(std::string szTexName) {
	uintptr_t pRwTexture = ((uintptr_t(*)(char const*))(g_libGTASA + 0x001BE990 + 1))(szTexName.c_str());
	if (pRwTexture) {
		++* (uint32_t*)(pRwTexture + 0x54);
	}
	return pRwTexture;
}

float GetDistanceFromVectorToVector(VECTOR* vecFrom, VECTOR* vecTo) {
	float fX = (vecFrom->X - vecTo->X) * (vecFrom->X - vecTo->X);
	float fY = (vecFrom->Y - vecTo->Y) * (vecFrom->Y - vecTo->Y);
	float fZ = (vecFrom->Z - vecTo->Z) * (vecFrom->Z - vecTo->Z);

	return (float)sqrt(fX + fY + fZ);
}
bool IsValidPosition(VECTOR const& vecPosition) {
	if (vecPosition.X < -16000 || vecPosition.X > 16000 || std::isnan(vecPosition.X) ||
		vecPosition.Y < -16000 || vecPosition.Y > 16000 || std::isnan(vecPosition.Y) ||
		vecPosition.Z < -5000 || vecPosition.Z > 100000 || std::isnan(vecPosition.Z)) {
		return false;
	}
	return true;
}

void DeleteRWTexture(uintptr_t* rwTexture)
{
	((void(*)(uintptr_t*))(g_libGTASA + 0x1B1808 + 1))(rwTexture);
}


RpMaterial* ObjectMaterialCallBack(RpMaterial* material, void* data)
{	
	CObject* pObject = (CObject*)data;
	RpAtomic* rpAtomic = (RpAtomic*)pObject->m_pEntity->m_pRpAtomic;
	for (int i = 0; i < rpAtomic->geometry->matList.numMaterials; i++)
	{
		if(i >= 16) break;
		if(rpAtomic->geometry->matList.materials[i] == material)
		{
			if(pObject->m_MaterialTexture[i]) material->texture = pObject->m_MaterialTexture[i];
			/*material->color.red = pObject->m_dwMaterialColor[i]->red;
			material->color.green = pObject->m_dwMaterialColor[i]->green;
			material->color.blue = pObject->m_dwMaterialColor[i]->blue;
			material->color.alpha = pObject->m_dwMaterialColor[i]->alpha;*/
			break;
		}
	}
	
	return material;
}

RpAtomic* ObjectMaterialTextCallBack(RpAtomic* rpAtomic, CObject* pObject)
{	
	if(!pObject->m_MaterialTextTexture || rpAtomic->object.object.type != 1) return rpAtomic;

	int iTotalEntries = rpAtomic->geometry->matList.numMaterials;
	if (iTotalEntries > 16) iTotalEntries = 16; // fix fucking bug :|
	for (int i = 0; i < iTotalEntries; i++)
	{
		if(pObject->m_MaterialTextTexture[i]) 
		{	
			rpAtomic->geometry->matList.materials[i]->texture = pObject->m_MaterialTextTexture[i];
		}
	}

	return rpAtomic;
}

float GetDistance(VECTOR *vec1, VECTOR *vec2)
{
    float fX = (vec1->X - vec2->X) * (vec1->X - vec2->X);
    float fY = (vec1->Y - vec2->Y) * (vec1->Y - vec2->Y);
    float fZ = (vec1->Z - vec2->Z) * (vec1->Z - vec2->Z);

    return (float)sqrt(fX + fY + fZ);
}

int iBuildingToRemoveCount = 0;
std::list<REMOVE_BUILDING_DATA> RemoveBuildingData;

void RemoveBuilding(unsigned short usModelToRemove, float fRange, float fX, float fY, float fZ)
{
    REMOVE_BUILDING_DATA entry;
    entry.usModelIndex = usModelToRemove;
    entry.fRange = fRange;
    entry.vecPos.X = fX;
    entry.vecPos.Y = fY;
    entry.vecPos.Z = fZ;

    iBuildingToRemoveCount++;

    RemoveBuildingData.push_back(entry);
}

void GamePrepareTrain(VEHICLE_TYPE *pVehicle)
{
    PED_TYPE *pDriver = pVehicle->pDriver;

    // GET RID OF THE PED DRIVER CREATED
    if(pDriver)
    {
        if (pDriver->dwPedType >= 2)
        {
            (( void (*)(PED_TYPE*))(*(void**)(pDriver->entity.vtable + 0x4)))(pDriver);
            pVehicle->pDriver = 0;
        }
    }
}

void util::emu_GammaSet(bool b)
{
	CPatch::CallFunction<void>(g_libGTASA + 0x198010 + 1, b);
}

bool util::IsValidGameVehicle(VEHICLE_TYPE *pVehicle) 
{
	// IsVehiclePointerValid
	return (((bool (*)(VEHICLE_TYPE *))(g_libGTASA+0x5109E8+1))(pVehicle));
}

bool util::IsObjectInCdImage(int ModelID)
{
	return CPatch::CallFunction<bool>(g_libGTASA + 0x28EAE8 + 1, ModelID);
}

bool util::IsValidModelVehicle(int iModelID)
{
	if(iModelID >= 400 && iModelID <= 611) return true;
	else return false;
}

// 0.3.7
float util::fixAngle(float angle)
{
	if (angle > 180.0f) angle -= 360.0f;
	if (angle < -180.0f) angle += 360.0f;

	return angle;
}
// 0.3.7
float util::subAngle(float a1, float a2)
{
	return fixAngle(fixAngle(a2) - a1);
}

eWidgetIds util::widgets::GetId(WIDGET_TYPE* pWidget)
{	
	if(!pWidget) return TYPE_NONE;

	pWidgets.Sprint = *(WIDGET_TYPE**)(g_libGTASA + 0x657EC4);
	pWidgets.Attack = *(WIDGET_TYPE**)(g_libGTASA + 0x657E4C);

	if(pWidget == pWidgets.Attack) return TYPE_ATTACK;
	if(pWidget == pWidgets.Sprint) return TYPE_SPRINT;
	if(pWidget == pWidgets.Accelerate) return TYPE_ACCELERATE;
	if(pWidget == pWidgets.EnterCar) return TYPE_ENTERCAR;

	return TYPE_NONE;
}

WIDGET_TYPE* util::widgets::GetById(int WidgetId)
{
	switch (WidgetId)
	{
	case TYPE_ATTACK:
		return pWidgets.Attack;
		break;
	case TYPE_SPRINT:
		return pWidgets.Sprint;
		break;
	case TYPE_ACCELERATE:
		return pWidgets.Accelerate;
		break;
	case TYPE_ENTERCAR:
		return pWidgets.EnterCar;
		break;
	default:
		break;
	}
}

void util::widgets::SetByName(const char* name, WIDGET_TYPE* pWidget)
{	

	if(!strcmp("accelerate", name)) util::widgets::SetById(TYPE_ACCELERATE, pWidget);
	if(!strcmp("hud_car", name)) util::widgets::SetById(TYPE_ENTERCAR, pWidget);
}

void util::widgets::SetById(int idWidget, WIDGET_TYPE* pWidget)
{
	switch (idWidget)
	{
	case TYPE_SPRINT:
		pWidgets.Sprint = pWidget;
		break;
	case TYPE_ACCELERATE:
		pWidgets.Accelerate = pWidget;
		break;
	case TYPE_ENTERCAR:
		pWidgets.EnterCar = pWidget;
		break;
	default:
		break;
	}
}

eWidgetState util::widgets::ProcessFixed(WIDGET_TYPE* pWidget)
{
	eWidgetIds idWidget = util::widgets::GetId(pWidget);
	switch (idWidget)
	{
	case TYPE_NONE:
		return STATE_NONE;
	case TYPE_ATTACK:
		if(pGame->FindPlayerPed()->IsInVehicle()) return STATE_FIXED;
		break;
	case TYPE_SPRINT:
		if(pGame->FindPlayerPed()->IsInVehicle()) return STATE_FIXED;
		break;
	case TYPE_ACCELERATE:
		if(!pGame->FindPlayerPed()->IsInVehicle()) return STATE_FIXED;
		break;
	case TYPE_ENTERCAR:
		VEHICLEID idPoolVeh = pNetGame->GetVehiclePool()->FindNearestToLocalPlayerPed();
		if(idPoolVeh != INVALID_VEHICLE_ID) 
			if(
				!pGame->FindPlayerPed()->IsInVehicle() && 
				GetDistance(&pGame->FindPlayerPed()->m_pEntity->mat->pos, &pNetGame->GetVehiclePool()->GetAt(idPoolVeh)->m_pEntity->mat->pos) > 10.0f) 
					return STATE_FIXED;
		break;
	}

	return STATE_NONE;
}

bool util::widgets::IsEnabled(WIDGET_TYPE* pWidget)
{
	return pWidget->bEnabled;
}

RwTexture* util::texture::CreatePlateTexture(const char* text)
{
	return CPatch::CallFunction<RwTexture*>(g_libGTASA + 0x53021C + 1, text, strlen(text));
}

RwCamera* util::game::GetCamera()
{
	return *(RwCamera**)(g_libGTASA + 0x95B064);
}

RpWorld* util::game::GetWorldScene()
{	
	return *(RpWorld**)(g_libGTASA + 0x95B060);
}

void util::game::OnNewGameCheck()
{
	CPatch::CallFunction<void>(g_libGTASA + 0x261C8C + 1);
}

void util::game::DefinedState()
{
	CPatch::CallFunction<void>(g_libGTASA + 0x559008 + 1);
}

void util::game::DefinedState2d()
{
	CPatch::CallFunction<void>(g_libGTASA + 0x5590B0 + 1);
}

void util::game::SetRenderWareCamera(RwCamera* camera)
{
	return CPatch::CallFunction<void>(g_libGTASA + 0x55CFA4 + 1, camera);
}

PLAYER_INFO* util::game::GetLocalPlayerInfo()
{
	return (PLAYER_INFO*)PLAYERS_REALLOC;
}

PED_TYPE* util::game::pool::FindPlayerPed()
{
	return *(PED_TYPE **) PLAYERS_REALLOC;
}

uintptr_t GetWeaponInfo(int iWeapon, int iSkill)
{
	// CWeaponInfo::GetWeaponInfo
	return (( uintptr_t (*)(int, int))(g_libGTASA+0x56BD60+1))(iWeapon, iSkill);
}

void RwMatrixOrthoNormalize(MATRIX4X4 *matIn, MATRIX4X4 *matOut)
{
	(( void (*)(MATRIX4X4*, MATRIX4X4*))(g_libGTASA+0x1B8CC8+1))(matIn, matOut);
}

void RwMatrixInvert(MATRIX4X4 *matOut, MATRIX4X4 *matIn)
{
	(( void (*)(MATRIX4X4*, MATRIX4X4*))(g_libGTASA+0x1B91CC+1))(matOut, matIn);
}

float ModelInfoGetColSphereRadius(int iModelID)
{
	uintptr_t modelInfo = GetModelInfoByID(iModelID);
	if(modelInfo)
	{
		uintptr_t colModel = *(uintptr_t *)(modelInfo+0x2C);
		if(colModel != 0)
			return *(float *)(colModel+0x24);
	}

	return 0.0f;
}

bool SphereCastVsEntity(int CColSphere1, int CColSphere2, ENTITY_TYPE *_pEntity, bool bTrue)
{
	uint32_t dwSavedgpColCache = 0;
	if(bTrue)
	{
		dwSavedgpColCache = *(uint32_t*)(g_libGTASA + 0x6F42D4);
		*(uint32_t*)(g_libGTASA + 0x6F42D4) = 0;
	}

	// CCollision::SphereCastVsEntity
	bool bSphereCastVsEntity = (( bool (*)(int, int, ENTITY_TYPE*))(g_libGTASA + 0x29C750 + 1))(CColSphere1, CColSphere2, _pEntity);

	if(bTrue)
		*(uint32_t*)(g_libGTASA + 0x6F42D4) = dwSavedgpColCache;

	return bSphereCastVsEntity;
}
MATRIX4X4 *RwMatrixMultiplyByVector(VECTOR *out, MATRIX4X4 *a2, VECTOR *in)
{
  float v3; // s0
  float v4; // s4
  float v5; // s6

  v3 = in->Y;
  v4 = in->Z;
  out->X = a2->pos.X + (a2->at.X * v4) + (a2->up.X * v3) + (a2->right.X * in->X);
  v5 = in->X;
  out->Y = a2->pos.Y + (a2->at.Y * v4) + (a2->up.Y * v3) + (a2->right.Y * in->X);
  out->Z = a2->pos.Z + (a2->at.Z * v4) + (a2->up.Z * in->Y) + (a2->right.Z * v5);
  return a2;
}