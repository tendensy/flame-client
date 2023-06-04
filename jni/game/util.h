#pragma once
class CObject;

PED_TYPE* GamePool_FindPlayerPed();
PED_TYPE* GamePool_Ped_GetAt(int iID);
int GamePool_Ped_GetIndex(PED_TYPE *pActor);

VEHICLE_TYPE *GamePool_Vehicle_GetAt(int iID);
int GamePool_Vehicle_GetIndex(VEHICLE_TYPE *pVehicle);

ENTITY_TYPE *GamePool_Object_GetAt(int iID);

int LineOfSight(VECTOR* start, VECTOR* end, void* colpoint, uintptr_t ent,
	char buildings, char vehicles, char peds, char objects, char dummies, bool seeThrough, bool camera, bool unk);

bool IsPedModel(unsigned int uiModel);
bool IsValidModel(unsigned int uiModelID);
uint16_t GetModelReferenceCount(int nModelIndex);

int FireSniper(WEAPON_SLOT_TYPE* pWeaponSlot, PED_TYPE* pPed);
uintptr_t LoadFromTxdSlot(const char* szSlot, const char* szTexture);
int GetWeaponSkill(PED_TYPE* pPed);

bool IsValidGameEntity(ENTITY_TYPE *pEntity);
bool IsGameEntityArePlaceable(ENTITY_TYPE *pEntity);

void InitPlayerPedPtrRecords();
void SetPlayerPedPtrRecord(uint8_t bytePlayer, uintptr_t dwPedPtr);
uint8_t FindPlayerNumFromPedPtr(uintptr_t dwPedPtr);
bool PreloadAnimFile(char *szAnimFile, uint16_t usSleep, uint16_t usHowTimes);
bool IsValidGamePed(PED_TYPE *pPed);

uintptr_t FindRwTexture(std::string szTexDb, std::string szTexName);
uintptr_t GetTexture(std::string szTexName);
uintptr_t LoadTextureFromDB(const char* dbname, const char* texture);

void SetScissorRect(void* pRect);
float DegToRad(float fDegrees);
// 0.3.7
float FloatOffset(float f1, float f2);

bool IsPointInRect(float x, float y, RECT* rect);

/// kek
void RwMatrixOrthoNormalize(MATRIX4X4 *matIn, MATRIX4X4 *matOut);
void RwMatrixInvert(MATRIX4X4 *matOut, MATRIX4X4 *matIn);
void ProjectMatrix(VECTOR* vecOut, MATRIX4X4* mat, VECTOR *vecPos);
void RwMatrixRotate(MATRIX4X4 *mat, int axis, float angle);
void RwMatrixScale(MATRIX4X4 *mat, VECTOR *vecScale);

int GetFreeTextDrawTextureSlot();
void DestroyTextDrawTexture(int index);
uintptr_t LoadTexture(const char* texname);
void DrawTextureUV(uintptr_t texture, RECT* rect, uint32_t dwColor, float *uv);

void WorldAddEntity(uintptr_t pEnt);
void WorldRemoveEntity(uintptr_t pEnt);
uintptr_t GetModelInfoByID(int iModelID);

void DestroyAtomicOrClump(uintptr_t rwObject);
void GetModelColSphereVecCenter(int iModel, VECTOR* vec);
float GetModelColSphereRadius(int iModel);
void RenderClumpOrAtomic(uintptr_t rwObject);
uintptr_t ModelInfoCreateInstance(int iModel);

float GetDistanceFromVectorToVector(VECTOR* vecFrom, VECTOR* vecTo);
bool IsValidPosition(VECTOR const& vecPosition);

void DeleteRWTexture(uintptr_t* rwTexture);

float GetDistance(VECTOR *vec1, VECTOR *vec2);
void RemoveBuilding(unsigned short usModelToRemove, float fRange, float fX, float fY, float fZ);
void GamePrepareTrain(VEHICLE_TYPE *pVehicle);

uintptr_t GetWeaponInfo(int iWeapon, int iSkill);

uintptr_t LoadTextureFromTxd(const char* database, const char *texture);

int Game_PedStatPrim(int model_id);

RpMaterial* ObjectMaterialCallBack(RpMaterial* material, void* data);
RpAtomic* ObjectMaterialTextCallBack(RpAtomic* rpAtomic, CObject* pObject);

float ModelInfoGetColSphereRadius(int iModelID);
bool SphereCastVsEntity(int CColSphere1, int CColSphere2, ENTITY_TYPE *_pEntity, bool bTrue);

enum eWidgetIds
{	
	TYPE_NONE = 0,
	TYPE_ATTACK,
	TYPE_SPRINT,
	TYPE_SWIM,
	TYPE_ENTERCAR,
	TYPE_ACCELERATE
};

enum eWidgetState
{
	STATE_NONE = 0,
	STATE_FIXED,
	STATE_NEEDEDFIX	
};

namespace util
{
	class texture
	{
	public:
		static RwTexture* CreatePlateTexture(const char* text);
	};
		
	namespace game
	{	
		RwCamera* 	GetCamera();
		RpWorld* 	GetWorldScene();
		
		void SetRenderWareCamera(RwCamera* camera);
		void OnNewGameCheck();
		
		void DefinedState();
		void DefinedState2d();

		PLAYER_INFO* GetLocalPlayerInfo();

		namespace pool
		{
			PED_TYPE* FindPlayerPed();
		}
	} 

	struct sWidgets
	{	
		WIDGET_TYPE* Attack = 0;
		WIDGET_TYPE* Sprint = 0;
		WIDGET_TYPE* Accelerate = 0;
		WIDGET_TYPE* EnterCar = 0;
	};

	float fixAngle(float angle);
	float subAngle(float a1, float a2);
	

	bool IsValidModelVehicle(int iModelID);
	bool IsObjectInCdImage(int ModelID);

	void emu_GammaSet(bool b);
	bool IsValidGameVehicle(VEHICLE_TYPE *pVehicle);

	namespace widgets
	{
		eWidgetIds GetId(WIDGET_TYPE* pWidget);
		WIDGET_TYPE* GetById(int WidgetId);
		void SetById(int idWidget, WIDGET_TYPE* pWidget);
		void SetByName(const char* name, WIDGET_TYPE* pWidget);
		bool IsEnabled(WIDGET_TYPE* pWidget);
		eWidgetState ProcessFixed(WIDGET_TYPE* pWidget);
	}

	namespace radar
	{
		float newLimitRadarPoint(float* pos);

		extern bool bSquare;
	}

	extern sWidgets pWidgets;
} 
MATRIX4X4 *RwMatrixMultiplyByVector(VECTOR *out, MATRIX4X4 *a2, VECTOR *in);
struct RwRaster* GetRWRasterFromBitmap(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY);
struct RwRaster* GetRWRasterFromBitmapPalette(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

