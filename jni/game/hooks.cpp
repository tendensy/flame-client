#include "../main.h"
#include "../util/patch.h"
#include "../game/RW/RenderWare.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../gui/gui.h"
#include "../chatwindow.h"
#include "../dialog.h"
#include "../scoreboard.h"
#include "../settings.h"
#include "loadingscreen.h"
#include "fpsfix.h"
#include "gui/gamescreen.h"
#include "../game/materialtext.h"
#include "../game/radar.h"
#include "firstperson.h"

//wtf man? what you doing?
CFPSFix* g_fps = nullptr;

extern CSettings* pSettings;
extern CGame* pGame;
extern CNetGame* pNetGame;
extern CGUI* pGUI;
extern CChatWindow* pChatWindow;
extern CLoadingScreen *pLoadingScreen;
extern CScoreBoard *pScoreBoard;
extern CGameScreen *pGameScreen;
extern CMaterialText *pMaterialText;
extern CDialogWindow *pDialogWindow;

// CGame__Process_hook(don\t find)
// fix game crashes for snapdragon 439 and Redmi phones

extern uint8_t *pbyteCurrentPlayer;
extern uintptr_t dwCurPlayerActor;
extern uint8_t byteCurPlayer;
extern uint8_t byteInternalPlayer;
int iSetToLocalPlayerSkill;
int g_iLagCompensationMode = 0;
bool bSetToLocalPlayerSkill;

void InstallSCMHooks();

bool g_bPlaySAMP = false;

void MainLoop();
void HookCPad();

extern "C" uintptr_t get_lib()
{
	return g_libGTASA;
}

struct stFile
{
	int isFileExist;
	FILE* f;
};

//TODO: FIX CRASH FROM NV
#include "../checkfilehash.h"
extern cryptor::string_encryptor SAMPFiles[9];
stFile* (*NvFOpen)(const char*, const char*, int, int); // int, int
stFile* NvFOpen_hook(const char* r0, const char* r1, int r2, int r3)// int r2, int r3
{
	char path[0xFF] = { 0 };

	//Log("[NvFOpen] %s", r1);
	// ----------------------------
	if (!strncmp(r1 + 12, cryptor::create("mainV1.scm", 11).decrypt(), 10))
	{
		sprintf(path, SAMPFiles[0].decrypt(), g_pszStorage);
		Log("Loading %s..", SAMPFiles[0].decrypt());
		goto open;
	}
	// ----------------------------
	if (!strncmp(r1 + 12, cryptor::create("SCRIPTV1.IMG", 13).decrypt(), 12))
	{
		sprintf(path, SAMPFiles[1].decrypt(), g_pszStorage);
		Log("Loading %s.", SAMPFiles[1].decrypt());
		goto open;
	}
	// ----------------------------
	if (!strncmp(r1, cryptor::create("DATA/PEDS.IDE", 14).decrypt(), 13))
	{
		sprintf(path, SAMPFiles[4].decrypt(), g_pszStorage);
		Log("Loading %s..", SAMPFiles[4].decrypt());
		goto open;
	}
	// ----------------------------
	if (!strncmp(r1, cryptor::create("DATA/GTA.DAT", 13).decrypt(), 12))
	{
		sprintf(path, SAMPFiles[8].decrypt(), g_pszStorage);
		Log("Loading %s..", SAMPFiles[8].decrypt());
		goto open;
	}
	if (!strcmp(r1, cryptor::create("DATA/VEHICLES.IDE", 18).decrypt()))
	{
		sprintf(path, SAMPFiles[2].decrypt(), g_pszStorage);
		Log("Loading %s..", SAMPFiles[2].decrypt());
		goto open;
	}
	if(!strcmp(r1, cryptor::create("DATA/HANDLING.CFG", 18).decrypt()))
	{
		sprintf(path, SAMPFiles[3].decrypt(), g_pszStorage);
		Log("Loading %s..", SAMPFiles[3].decrypt());
		goto open;
	}
	// ----------------------------
	if(!strcmp(r1, cryptor::create("DATA/WEAPON.DAT", 16).decrypt()))
	{
		sprintf(path, SAMPFiles[7].decrypt(), g_pszStorage);
		Log("Loading %s..", SAMPFiles[7].decrypt());
		goto open;
	}

orig:
	return NvFOpen(r0, r1, r2, r3);

open:
	stFile* st = (stFile*)malloc(8);
	st->isFileExist = false;

	FILE* f = fopen(path, "rb");
	if (f)
	{
		st->isFileExist = true;
		st->f = f;
		return st;
	}
	else
	{
		Log("NVFOpen hook | Error: file not found (%s)", path);
		free(st);
		st = nullptr;
		return 0;
	}
}

int (*OS_FileOpen)(int a1, int a2, const char *a3, int a4);
int OS_FileOpen_hook(int a1, int a2, const char *a3, int a4)
{
           int v8; // r0
           int v9; // r4
           char v11[256]; 
           /*
                    [sp+0h] [bp-120h] BYREF
           */

           memset(v11, 0, sizeof(v11));
           if(!strncmp(a3, "scache.txt", 0xAu) || !strncmp(a3, "scache_small_low.txt", 0x14u) || !strncmp(a3, "scache_small.txt", 0x10u))
           {
                     return 0;
           }

           if (strncasecmp(a3, "DATA\\", 5u)
           || !strncasecmp(a3, "DATA\\360Default960x720.cfg", 0x1Au)
           || !strncasecmp(a3, "DATA\\360Default1280x720.cfg", 0x1Bu)
           || !strncasecmp(a3, "DATA\\TouchDefaultPhone3x2.cfg", 0x1Du)
           || !strncasecmp(a3, "DATA\\TouchDefaultPhoneWidescreen.cfg", 0x24u)
           || !strncasecmp(a3, "DATA\\TouchDefaultTablet4x3.cfg", 0x1Eu)
           || !strncasecmp(a3, "DATA\\TouchDefaultTabletWidescreen.cfg", 0x25u)
           || !strncasecmp(a3, "DATA\\colorcycle.dat", 0x13u)
           || !strncasecmp(a3, "DATA\\timecyc.dat", 0x10u))
           {
                   return OS_FileOpen(a1, a2, a3, a4);
           }
           sprintf(v11, "\\%s", a3);
           v8 = OS_FileOpen(a1, a2, v11, a4);

           if (!v8)
           return 0;

           v9 = v8;

           Log("OS_FileOpen_hook | Error: file not found (%s), %d", v11, v8);
           return v9;
}

void(*CHud__Draw)();
void CHud__Draw_hook()
{
	float* thiz = (float*) * (uintptr_t*)(g_libGTASA + 0x6580C8);
	if (thiz)
	{
		thiz[5] = 45.0f;
		thiz[6] = 45.0f;
	}
	CHud__Draw();
}

// to do fix java lags pr
int Init_hook(int r0, int r1, int r2)
{
	int result = ((int (*)(int, int, int))(g_libGTASA + 0x244F2C + 1))(r0, r1, r2);

	CMain::InitSAMP();

	if ( !*(uintptr_t *)(g_libGTASA + 0x61B298) && !((int (*)(const char *))(g_libGTASA + 0x179A20))("glAlphaFuncQCOM") )
  	{
    	       CPatch::NOP(g_libGTASA + 0x1A6164, 4);
    	       CPatch::WriteMemory(g_libGTASA + 0x1A6164, "\x70\x47", 2);
  	}

	return result;
}

bool bGameStarted = false;
void (*CGame__Process)();
void CGame__Process_hook()
{
	CGame__Process();
	bGameStarted = true;
	if(pGame->IsMenuActive()) return;

	MainLoop();
	if (pNetGame)
	{
		CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
		if (pTextDrawPool) pTextDrawPool->SnapshotProcess();
	}	
}


uint16_t gxt_string[0x7F];
uint16_t* (*CText_Get)(uintptr_t thiz, const char* szText);
uint16_t* CText_Get_hook(uintptr_t thiz, const char* text)
{
	if(text[0] == 'S' && text[1] == 'A' && text[2] == 'M' && text[3] == 'P')
	{
		const char* code = &text[5];
		if(!strcmp(code, "MP")) CFont::AsciiToGxtChar("MultiPlayer", gxt_string);

    	return gxt_string;
	}

	return CText_Get(thiz, text);
}

/* ====================================================== */

// CGame::InitialiseRenderWare
void (*InitialiseRenderWare)();
void InitialiseRenderWare_hook()
{
	Log("Loading \"samp\" cd..");

	InitialiseRenderWare();
	// TextureDatabaseRuntime::Load()
	((void (*)(const char*, int, int))(g_libGTASA + 0x1BF244 + 1))("samp", 0, 5);
	//((void (*)(const char*, int, int))(g_libGTASA + 0x1BF244 + 1))("flame", 0, 5);
	return;
}

/* ====================================================== */

int (*CMainMenu__Update)(uintptr_t thiz, float a2);
int CMainMenu__Update_hook(uintptr_t thiz, float a2)
{
	if(!g_bPlaySAMP)
	{
		int result = CMainMenu__Update(thiz, a2);

		CMain::InitInMenu();
		util::game::OnNewGameCheck();

		g_bPlaySAMP = true;
		return result;
	}
	return CMainMenu__Update(thiz, a2);
}

/* ====================================================== */

void (*CLoadingScreen_DisplayPCScreen)();
void CLoadingScreen_DisplayPCScreen_hook()
{
	RwCamera* camera = util::game::GetCamera();

	if (RwCameraBeginUpdate(camera))
	{
		util::game::DefinedState2d();
		CSprite2d::InitPerFrame();
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);
		util::emu_GammaSet(false);
		pLoadingScreen->Render();
		RwCameraEndUpdate(camera);
		RwCameraShowRaster(camera, 0, 0);
	}

	return;
}

/* ====================================================== */

void (*TouchEvent)(int, int, int posX, int posY);
void TouchEvent_hook(int type, int num, int posX, int posY)
{
	if(pGUI) {
		bool bRet = pGUI->OnTouchEvent(type, num, posX, posY);
		if(bRet) {
			TouchEvent(type, num, posX, posY);
		}
		return;
	}

	return TouchEvent(type, num, posX, posY);
}

/* ====================================================== */

void (*CStreaming_InitImageList)();
void CStreaming_InitImageList_hook()
{
	char* ms_files = (char*)(g_libGTASA + 0x6702FC);
	ms_files[0] = 0;
	*(uint32_t*)&ms_files[44] = 0;
	ms_files[48] = 0;
	*(uint32_t*)&ms_files[92] = 0;
	ms_files[96] = 0;
	*(uint32_t*)&ms_files[140] = 0;
	ms_files[144] = 0;
	*(uint32_t*)&ms_files[188] = 0;
	ms_files[192] = 0;
	*(uint32_t*)&ms_files[236] = 0;
	ms_files[240] = 0;
	*(uint32_t*)&ms_files[284] = 0;
	ms_files[288] = 0;
	*(uint32_t*)&ms_files[332] = 0;
	ms_files[336] = 0;
	*(uint32_t*)&ms_files[380] = 0;

	pLoadingScreen->SetLoadingText("Loading IMG`s");

	CStreaming::AddImageToList("TEXDB\\GTA3.IMG", true);
	CStreaming::AddImageToList("TEXDB\\GTA_INT.IMG", true);
	CStreaming::AddImageToList("TEXDB\\SAMP.IMG", true);
	CStreaming::AddImageToList("TEXDB\\SAMPCOL.IMG", true);
}

/* ====================================================== */

typedef struct _PED_MODEL
{
	uintptr_t 	vtable;
	uint8_t		data[88];
} PED_MODEL; // SIZE = 92

PED_MODEL PedsModels[315];
int PedsModelsCount = 0;

PED_MODEL* (*CModelInfo_AddPedModel)(int id);
PED_MODEL* CModelInfo_AddPedModel_hook(int id)
{
	//Log("loading skin model %d", id);

	PED_MODEL* model = &PedsModels[PedsModelsCount];
	memset(model, 0, sizeof(PED_MODEL));
	model->vtable = (uintptr_t)(g_libGTASA + 0x5C6E90);

	// CClumpModelInfo::CClumpModelInit()
	((uintptr_t(*)(PED_MODEL*))(*(void**)(model->vtable + 0x1C)))(model);

	*(PED_MODEL**)(g_libGTASA + 0x87BF48 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	PedsModelsCount++;
	return model;
}
/* ====================================================== */
void (*CPools_Initialise)(void);
void CPools_Initialise_hook(void)
{
	struct PoolAllocator {

		struct Pool {
			void* objects;
			uint8_t* flags;
			uint32_t count;
			uint32_t top;
			uint32_t bInitialized;
		};
		static_assert(sizeof(Pool) == 0x14);

		static Pool* Allocate(size_t count, size_t size) {

			Pool* p = new Pool;

			p->objects = new char[size * count];
			p->flags = new uint8_t[count];
			p->count = count;
			p->top = 0xFFFFFFFF;
			p->bInitialized = 1;

			for (size_t i = 0; i < count; i++) {
				p->flags[i] |= 0x80;
				p->flags[i] &= 0x80;
			}

			return p;
		}
	};

	pLoadingScreen->SetLoadingText("Initialise pools");

	// 600000 / 75000 = 8
	static auto ms_pPtrNodeSingleLinkPool = PoolAllocator::Allocate(100000, 8);		// 75000
	// 72000 / 6000 = 12
	static auto ms_pPtrNodeDoubleLinkPool = PoolAllocator::Allocate(60000, 12);	// 6000
	// 10000 / 500 = 20
	static auto ms_pEntryInfoNodePool = PoolAllocator::Allocate(20000, 20);	// 500
	// 279440 / 140 = 1996
	static auto ms_pPedPool = PoolAllocator::Allocate(240, 1996);	// 140
	// 286440 / 110 = 2604
	static auto ms_pVehiclePool = PoolAllocator::Allocate(2000, 2604);	// 110
	// 840000 / 14000 = 60
	static auto ms_pBuildingPool = PoolAllocator::Allocate(20000, 60);	// 14000
	// 147000 / 350 = 420
	static auto ms_pObjectPool = PoolAllocator::Allocate(3000, 420);	// 350
	// 210000 / 3500 = 60
	static auto ms_pDummyPool = PoolAllocator::Allocate(40000, 60);	// 3500
	// 487200 / 10150 = 48
	static auto ms_pColModelPool = PoolAllocator::Allocate(50000, 48);	// 10150
	// 64000 / 500 = 128
	static auto ms_pTaskPool = PoolAllocator::Allocate(5000, 128);	// 500
	// 13600 / 200 = 68
	static auto ms_pEventPool = PoolAllocator::Allocate(1000, 68);	// 200
	// 6400 / 64 = 100
	static auto ms_pPointRoutePool = PoolAllocator::Allocate(200, 100);	// 64
	// 13440 / 32 = 420
	static auto ms_pPatrolRoutePool = PoolAllocator::Allocate(200, 420);	// 32
	// 2304 / 64 = 36
	static auto ms_pNodeRoutePool = PoolAllocator::Allocate(200, 36);	// 64
	// 512 / 16 = 32
	static auto ms_pTaskAllocatorPool = PoolAllocator::Allocate(3000, 32);	// 16
	// 92960 / 140 = 664
	static auto ms_pPedIntelligencePool = PoolAllocator::Allocate(240, 664);	// 140
	// 15104 / 64 = 236
	static auto ms_pPedAttractorPool = PoolAllocator::Allocate(200, 236);	// 64

	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93E0) = ms_pPtrNodeSingleLinkPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93DC) = ms_pPtrNodeDoubleLinkPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93D8) = ms_pEntryInfoNodePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93D4) = ms_pPedPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93D0) = ms_pVehiclePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93CC) = ms_pBuildingPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93C8) = ms_pObjectPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93C4) = ms_pDummyPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93C0) = ms_pColModelPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93BC) = ms_pTaskPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B8) = ms_pEventPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B4) = ms_pPointRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B0) = ms_pPatrolRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93AC) = ms_pNodeRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A8) = ms_pTaskAllocatorPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A4) = ms_pPedIntelligencePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A0) = ms_pPedAttractorPool;
}

/*void (*CWorld__ProcessPedsAfterPreRender)();
void CWorld__ProcessPedsAfterPreRender_hook()
{
	CWorld__ProcessPedsAfterPreRender();

	if (pNetGame)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

		if (pPlayerPool)
		{
			CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();

			if (pLocalPlayer)
			{
				CPlayerPed* pPlayerPed = pLocalPlayer->GetPlayerPed();
				if (pPlayerPed) {
					pPlayerPed->ProcessAttachedObjects();
				}
			}

			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
				if (pRemotePlayer)
				{
					CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();
					if (pPlayerPed) {
						pPlayerPed->ProcessAttachedObjects();
					}
				}
			}
		}
	}
}*/

void (*CWorld__ProcessPedsAfterPreRender)();
void CWorld__ProcessPedsAfterPreRender_hook()
{
	CWorld__ProcessPedsAfterPreRender();
	if(pNetGame)
	{
		CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
		if(pPlayerPool)
			pPlayerPool->ProcessAttachedObjects();
	}
}

struct _ATOMIC_MODEL
{
	uintptr_t func_tbl;
	char data[56];
};
extern struct _ATOMIC_MODEL* ATOMIC_MODELS;

uintptr_t(*CModelInfo_AddAtomicModel)(int iModel);
uintptr_t CModelInfo_AddAtomicModel_hook(int iModel)
{
	uint32_t iCount = *(uint32_t*)(g_libGTASA + 0x7802C4);
	_ATOMIC_MODEL* model = &ATOMIC_MODELS[iCount];
	*(uint32_t*)(g_libGTASA + 0x7802C4) = iCount + 1;

	((void(*)(_ATOMIC_MODEL*))(*(uintptr_t*)(model->func_tbl + 0x1C)))(model);
	_ATOMIC_MODEL** ms_modelInfoPtrs = (_ATOMIC_MODEL**)(g_libGTASA + 0x87BF48);
	ms_modelInfoPtrs[iModel] = model;
	return (uintptr_t)model;
}
/* ==== */

uint32_t(*CRadar__GetRadarTraceColor)(uint32_t color, uint8_t bright, uint8_t friendly);
uint32_t CRadar__GetRadarTraceColor_hook(uint32_t color, uint8_t bright, uint8_t friendly)
{
	return TranslateColorCodeToRGBA(color);
}

int (*CRadar__SetCoordBlip)(int r0, float X, float Y, float Z, int r4, int r5, char* name);
int CRadar__SetCoordBlip_hook(int r0, float X, float Y, float Z, int r4, int r5, char* name)
{
	if (pNetGame && !strncmp(name, "CODEWAY", 7))
	{
		float findZ = ((float (*)(float, float))(g_libGTASA + 0x3C3DD8 + 1))(X, Y);
		findZ += 1.5f;

		RakNet::BitStream bsSend;
		bsSend.Write(X);
		bsSend.Write(Y);
		bsSend.Write(findZ);
		pNetGame->GetRakClient()->RPC(&RPC_MapMarker, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}

	return CRadar__SetCoordBlip(r0, X, Y, Z, r4, r5, name);
}

uint8_t bGZ = 0;
void (*CRadar__DrawRadarGangOverlay)(uint8_t v1);
void CRadar__DrawRadarGangOverlay_hook(uint8_t v1)
{
	bGZ = v1;
	if (pNetGame && pNetGame->GetGangZonePool() && pGame->FindPlayerPed()->m_bPlayerControllable)
		pNetGame->GetGangZonePool()->Draw();
}

uint32_t dwParam;
extern "C" void doPickingUpPickup()
{
	if(pNetGame) {
		CPickupPool *pPickupPool = pNetGame->GetPickupPool();
		if(pPickupPool) {
			pPickupPool->PickedUp(((dwParam - (g_libGTASA + 0x0070E264)) / 0x20));
		}
	}
}
__attribute__((naked)) void PickupPickUp_hook()
{
	//LOGI("PickupPickUp_hook");

	// calculate and save ret address
	__asm__ volatile("push {lr}\n\t"
		"push {r0}\n\t"
		"blx get_lib\n\t"
		"add r0, #0x2D0000\n\t"
		"add r0, #0x009A00\n\t"
		"add r0, #1\n\t"
		"mov r1, r0\n\t"
		"pop {r0}\n\t"
		"pop {lr}\n\t"
		"push {r1}\n\t");

	//
	__asm__ volatile("push {r0-r11, lr}\n\t"
		"mov %0, r4" : "=r" (dwParam));

	__asm__ volatile("blx doPickingUpPickup\n\t");


	__asm__ volatile("pop {r0-r11, lr}\n\t");

	// restore
	__asm__ volatile("ldrb r1, [r4, #0x1C]\n\t"
		"sub.w r2, r1, #0xD\n\t"
		"sub.w r2, r1, #8\n\t"
		"cmp r1, #6\n\t"
		"pop {pc}\n\t");
}

extern "C" bool NotifyEnterVehicle(VEHICLE_TYPE * _pVehicle)
{
	//Log("NotifyEnterVehicle");

	if (!pNetGame) return false;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle* pVehicle;
	VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(_pVehicle);

	if (VehicleID == INVALID_VEHICLE_ID) return false;
	if (!pVehiclePool->GetSlotState(VehicleID)) return false;
	pVehicle = pVehiclePool->GetAt(VehicleID);
	if (pVehicle->m_bDoorsLocked) return false;
	if (pVehicle->m_pVehicle->entity.nModelIndex == TRAIN_PASSENGER) return false;

	if (pVehicle->m_pVehicle->pDriver &&
		pVehicle->m_pVehicle->pDriver->dwPedType != 0)
		return false;

	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	//if(pLocalPlayer->GetPlayerPed() && pLocalPlayer->GetPlayerPed()->GetCurrentWeapon() == WEAPON_PARACHUTE)
	//  pLocalPlayer->GetPlayerPed()->SetArmedWeapon(0);

	pLocalPlayer->SendEnterVehicleNotification(VehicleID, false);

	return true;
}

void (*CTaskComplexEnterCarAsDriver)(uint32_t thiz, uint32_t pVehicle);
extern "C" void call_taskEnterCarAsDriver(uintptr_t a, uint32_t b)
{
	CTaskComplexEnterCarAsDriver(a, b);
}
void __attribute__((naked)) CTaskComplexEnterCarAsDriver_hook(uint32_t thiz, uint32_t pVehicle)
{
	__asm__ volatile("push {r0-r11, lr}\n\t"
		"mov r2, lr\n\t"
		"blx get_lib\n\t"
		"add r0, #0x3A0000\n\t"
		"add r0, #0xEE00\n\t"
		"add r0, #0xF7\n\t"
		"cmp r2, r0\n\t"
		"bne 1f\n\t" // !=
		"mov r0, r1\n\t"
		"blx NotifyEnterVehicle\n\t" // call NotifyEnterVehicle
		"1:\n\t"  // call orig
		"pop {r0-r11, lr}\n\t"
		"push {r0-r11, lr}\n\t"
		"blx call_taskEnterCarAsDriver\n\t"
		"pop {r0-r11, pc}");
}

void (*CTaskComplexLeaveCar)(uintptr_t** thiz, VEHICLE_TYPE* pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut);
void CTaskComplexLeaveCar_hook(uintptr_t** thiz, VEHICLE_TYPE* pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x3AE905 || dwRetAddr == 0x3AE9CF)
	{
		if (pNetGame)
		{
			if (pGame->FindPlayerPed()->IsInVehicle())
			{
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
				VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(pGame->FindPlayerPed()->GetGtaVehicle());
				CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
				pLocalPlayer->SendExitVehicleNotification(VehicleID);
			}
		}
	}

	(*CTaskComplexLeaveCar)(thiz, pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut);
}

uint16_t(*CHud_DrawVitalStats)(uintptr_t thiz);
uint16_t CHud_DrawVitalStats_hook(uintptr_t thiz)
{
	if(pSettings && pScoreBoard && pNetGame && pGameScreen)
	{
		if(pSettings->Get().iFistClick == 0) pGameScreen->GetInterface()->ToggleMenuState(); // Меню клиента
		if(pSettings->Get().iFistClick == 1) pScoreBoard->Toggle(); // Список игроков
		if(pSettings->Get().iFistClick == 2) pNetGame -> SendChatCommand("/mystats"); // стата
		if(pSettings->Get().iFistClick == 3) pNetGame -> SendChatCommand("/time"); // время
		if(pSettings->Get().iFistClick == 4) pNetGame -> SendChatCommand("/actionmenu"); // Меню быстрых действий
	}
	return 0;
}

/*int (*CTaskSimpleUseGun__SetPedPosition)(uintptr_t thiz, uintptr_t a2);
int CTaskSimpleUseGun__SetPedPosition_hook(uintptr_t thiz, uintptr_t a2)
{
	//if(!thiz || !a2) return CTaskSimpleUseGun__SetPedPosition(thiz, a2);

	unsigned char v1 = *((unsigned char*)thiz + 13);
	bool bChangeTheResult = false;

	PED_TYPE* pPedPlayer = (PED_TYPE*)a2;
	if (pPedPlayer != 0 && pNetGame != 0)
	{
		if (v1 == 0)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if (pPlayerPool)
			{
				// LOCAL PED
				if (pPedPlayer == GamePool_FindPlayerPed())
				{
					CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
					if (pPlayerPed)
					{
						if ((pPlayerPed->GetCurrentWeapon() == 42 || pPlayerPed->GetCurrentWeapon() == 41) && pPlayerPed->GetGtaVehicle() == 0)
						{
							CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
							if (pVehiclePool)
							{
								for (VEHICLEID veh = 0; veh < MAX_VEHICLES; veh++)
								{
									if (pVehiclePool->GetSlotState(veh))
									{
										CVehicle* pVehicle = pVehiclePool->GetAt(veh);
										if (pVehicle)
										{
											MATRIX4X4 vehMat, playerMat;
											pVehicle->GetMatrix(&vehMat);
											pPlayerPed->GetMatrix(&playerMat);

											float fSX = (vehMat.pos.X - playerMat.pos.X) * (vehMat.pos.X - playerMat.pos.X);
											float fSY = (vehMat.pos.Y - playerMat.pos.Y) * (vehMat.pos.Y - playerMat.pos.Y);
											float fSZ = (vehMat.pos.Z - playerMat.pos.Z) * (vehMat.pos.Z - playerMat.pos.Z);

											float fDistance = (fSX + fSY + fSZ);

											if (fDistance <= 100.0f)
												bChangeTheResult = true;
										}
									}
								}
							}
						}
					}
				}
				// REMOTE PED
				else
				{
					PLAYERID remotePlayerID = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pPedPlayer);
					if (remotePlayerID != INVALID_PLAYER_ID)
					{
						CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(remotePlayerID);
						if (pRemotePlayer)
						{
							CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();
							if ((pPlayerPed->GetCurrentWeapon() == 42 || pPlayerPed->GetCurrentWeapon() == 41) && pPlayerPed->GetGtaVehicle() == 0)
							{
								CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
								if (pVehiclePool)
								{
									for (VEHICLEID veh = 0; veh < MAX_VEHICLES; veh++)
									{
										if (pVehiclePool->GetSlotState(veh))
										{
											CVehicle* pVehicle = pVehiclePool->GetAt(veh);
											if (pVehicle)
											{
												MATRIX4X4 vehMat, playerMat;
												pVehicle->GetMatrix(&vehMat);
												pPlayerPed->GetMatrix(&playerMat);

												float fSX = (vehMat.pos.X - playerMat.pos.X) * (vehMat.pos.X - playerMat.pos.X);
												float fSY = (vehMat.pos.Y - playerMat.pos.Y) * (vehMat.pos.Y - playerMat.pos.Y);
												float fSZ = (vehMat.pos.Z - playerMat.pos.Z) * (vehMat.pos.Z - playerMat.pos.Z);

												float fDistance = (fSX + fSY + fSZ);

												if (fDistance <= 100.0f)
													bChangeTheResult = true;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (bChangeTheResult)
		*((unsigned char*)thiz + 13) |= 1;

	return CTaskSimpleUseGun__SetPedPosition(thiz, a2);
}*/

int (*CTaskSimpleUseGun__SetPedPosition)(uintptr_t thiz, uintptr_t a2);
int CTaskSimpleUseGun__SetPedPosition_hook(uintptr_t thiz, uintptr_t a2)
{
	unsigned char v1 = *((unsigned char*)thiz + 13);
	bool bChangeTheResult = false;

	PED_TYPE* pPedPlayer = (PED_TYPE*)a2;
	if(pPedPlayer && pNetGame)
	{
		if(v1 == 0)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if(pPlayerPool)
			{
				CPlayerPed *pPlayerPed;
				if(pPedPlayer == GamePool_FindPlayerPed())
					pPlayerPed = pGame->FindPlayerPed();
				else
				{
					PLAYERID playerId = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pPedPlayer);
					if(playerId != INVALID_PLAYER_ID)
					{
						CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(playerId);
						if(pRemotePlayer)
							pPlayerPed = pRemotePlayer->GetPlayerPed();
					}
				}

				if(pPlayerPed)
				{
					if((pPlayerPed->GetCurrentWeapon() == 42 || pPlayerPed->GetCurrentWeapon() == 41) && pPlayerPed->GetGtaVehicle() == 0)
					{
						CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
						if(pVehiclePool)
						{
							for(VEHICLEID veh = 0; veh < MAX_VEHICLES; veh++)
							{
								if(pVehiclePool->GetSlotState(veh))
								{
									CVehicle* pVehicle = pVehiclePool->GetAt(veh);
									if(pVehicle)
									{
										MATRIX4X4 vehicleMat, playerMat;
										pVehicle->GetMatrix(&vehicleMat);
										pPlayerPed->GetMatrix(&playerMat);

										float fSX = (vehicleMat.pos.X - playerMat.pos.X) * (vehicleMat.pos.X - playerMat.pos.X);
										float fSY = (vehicleMat.pos.Y - playerMat.pos.Y) * (vehicleMat.pos.Y - playerMat.pos.Y);
										float fSZ = (vehicleMat.pos.Z - playerMat.pos.Z) * (vehicleMat.pos.Z - playerMat.pos.Z);

										float fDistance = (float)sqrt(fSX + fSY + fSZ);

										if(fDistance <= 100.0f)
											*((unsigned char*)thiz + 13) |= 1;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return CTaskSimpleUseGun__SetPedPosition(thiz, a2);
}

int (*CWeaponInfo__GetWeaponReloadTime)(uintptr_t thiz);
int CWeaponInfo__GetWeaponReloadTime_hook(uintptr_t thiz)
{
	int result = CWeaponInfo__GetWeaponReloadTime(thiz);

	if (pChatWindow) pChatWindow->AddDebugMessage("weapon reload time: %d", result);

	return result;
}

void (*CObject__Render)(uintptr_t thiz);
void CObject__Render_hook(uintptr_t thiz)
{	
	ENTITY_TYPE *object = (ENTITY_TYPE*)thiz;
	if(pNetGame && object != 0)
	{	
		

		CObject *pObject = pNetGame->GetObjectPool()->GetObjectFromGtaPtr(object);
		if(pObject)
		{
			RwObject* rwObject = (RwObject*)pObject->GetRWObject();
			if(rwObject
			)
			{	
				
                // SetObjectMaterial
                if(pObject->m_bHasMaterial)
					{
						//RwFrameForAllObjects((RwFrame*)rwObject->parent, (RwObject *(*)(RwObject *, void *))ObjectMaterialCallBack, pObject);
						RpAtomic* atomic = (RpAtomic*)object->m_pRpAtomic;
						//atomic->geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;
						RpGeometryForAllMaterials(atomic->geometry, ObjectMaterialCallBack, (void*)pObject);
						
            
					}
                // SetObjectMaterialText
                if(pObject->m_bHasMaterialText)
                    RwFrameForAllObjects((RwFrame*)rwObject->parent, (RwObject *(*)(RwObject *, void *))ObjectMaterialTextCallBack, pObject);
			}


		}
	}

	CPatch::CallFunction<void>(g_libGTASA + 0x559EF9);
	CObject__Render(thiz);
	CPatch::CallFunction<void>(g_libGTASA + 0x559F15);
}

extern int iBuildingToRemoveCount;
extern std::list<REMOVE_BUILDING_DATA> RemoveBuildingData;

void (*CEntity_Render)(ENTITY_TYPE* pEntity);
void CEntity_Render_hook(ENTITY_TYPE* pEntity)
{
    if(iBuildingToRemoveCount > 1)
    {
        if(pEntity && pEntity->vtable != g_libGTASA+0x5C7358 && pEntity->mat && !pNetGame->GetObjectPool()->GetObjectFromGtaPtr(pEntity))
        {
            for (auto &entry : RemoveBuildingData)
            {
                float fDistance = GetDistance(&entry.vecPos, &pEntity->mat->pos);
                if(fDistance <= entry.fRange)
                {
                    if(pEntity->nModelIndex == entry.usModelIndex)
                    {
						pEntity->nEntityFlags.m_bUsesCollision = 0;
						pEntity->nEntityFlags.m_bCollisionProcessed = 0;
                        return;
                    }
                }
            }
        }
    }
    return CEntity_Render(pEntity);
}

/*RpAtomic* (*CCustomRoadsignMgr_RenderRoadsignAtomic)(RpAtomic*, VECTOR const&);
RpAtomic* CCustomRoadsignMgr_RenderRoadsignAtomic_hook(RpAtomic* atomic, VECTOR const& a2)
{
	return atomic;
}*/

int (*CCustomRoadsignMgr__RenderRoadsignAtomic)(uintptr_t thiz, float a2);
int CCustomRoadsignMgr__RenderRoadsignAtomic_hook(uintptr_t thiz, float a2)
{
	if(!thiz) return 0;
	return CCustomRoadsignMgr__RenderRoadsignAtomic(thiz, a2);
}

/*RwTexture* (*CCustomCarPlateMgr__CreatePlateTexture)(const char*, uint8_t a2);
RwTexture* CCustomCarPlateMgr__CreatePlateTexture_hook(const char* text, uint8_t a2)
{
	RwRaster* raster = RwRasterCreate(64, 16, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	RwTexture* texture = RwTextureCreate(raster);
	return texture;
}*/

RwTexture* (*CCustomCarPlateMgr__CreatePlateTexture)(const char* text, uint8_t a2);
RwTexture* CCustomCarPlateMgr__CreatePlateTexture_hook(const char* text, uint8_t a2)
{
	return (RwTexture*)pMaterialText->GenerateNumberPlate(text);
}

// OsArray<FlowScreen::MenuItem>::Add
void (*MenuItem_add)(int r0, uintptr_t r1);
void MenuItem_add_hook(int r0, uintptr_t r1)
{
	char* name = *(char**)(r1 + 4);

	if (g_bPlaySAMP && (
		!strcmp(name, cryptor::create("FEP_STG", 7).decrypt()) ||
		!strcmp(name, cryptor::create("FEH_STA", 7).decrypt()) ||
		!strcmp(name, cryptor::create("FEH_BRI", 7).decrypt())))
		return;

ret:
	return MenuItem_add(r0, r1);
}

void (*MobileMenu__Render)(uintptr_t);
void MobileMenu__Render_hook(uintptr_t thiz)
{
	MobileMenu__Render(thiz);

	uint32_t dwReversed = __builtin_bswap32(0xFFFFFFFF);
	CFont::SetColor(&dwReversed);
	CFont::SetFontStyle(1);
	CFont::SetScaleXY(1.0, 2.0);
	CFont::SetCentreSize(0);
	if(pGUI)CFont::PrintString(pGUI->ScaleX(5), pGUI->ScaleY(5), cryptor::create("PLACEINT", 11).decrypt());
}

int (*SetCompAlphaCB)(int a1, char a2);
int SetCompAlphaCB_hook(int a1, char a2)
{
	/*if (!a1) return 0;
	return SetCompAlphaCB(a1, a2); OLD*/
	if (!a1) return 0;
	*(uint8_t*)(a1 + 7) = a2;
	return a1;
}

int (*_rwFreeListFreeReal)(int a1, unsigned int a2);
int _rwFreeListFreeReal_hook(int a1, unsigned int a2)
{
	if (a1 == 0 || !a1) return 0;
	return _rwFreeListFreeReal(a1, a2);
}

uintptr_t(*CPlayerPedDataSaveStructure__Construct)(int a1, int a2);
uintptr_t CPlayerPedDataSaveStructure__Construct_hook(int a1, int a2)
{
	if (!a1 || !a2) return 0;
	if (!*(int*)a2) return 0;
	return CPlayerPedDataSaveStructure__Construct(a1, a2);
}

int (*CAnimBlendNode__FindKeyFrame)(uintptr_t thiz, float a2, int a3, int a4);
int CAnimBlendNode__FindKeyFrame_hook(uintptr_t thiz, float a2, int a3, int a4)
{
	if (!thiz || !*((uintptr_t*)thiz + 4)) return 0;
	return CAnimBlendNode__FindKeyFrame(thiz, a2, a3, a4);
}

int (*CTextureDatabaseRuntime__GetEntry)(uintptr_t thiz, const char* a2, bool* a3);
int CTextureDatabaseRuntime__GetEntry_hook(uintptr_t thiz, const char* a2, bool* a3)
{
	if (!thiz)
	{
		return -1;
	}
	return CTextureDatabaseRuntime__GetEntry(thiz, a2, a3);
}

static uint32_t dwRLEDecompressSourceSize = 0;

/*
int (*RLEDecompress)(int a1, unsigned int a2, const char* a3, unsigned int a4, unsigned int a5);
int RLEDecompress_hook(int a1, unsigned int a2, const char* a3, unsigned int a4, unsigned int a5)
{
	if (!a3) return 0;
	return RLEDecompress(a1, a2, a3, a4, a5);
}*/

uint8_t* (*RLEDecompress)(uint8_t* pDest, size_t uiDestSize, uint8_t const* pSrc, size_t uiSegSize, uint32_t uiEscape);
uint8_t* RLEDecompress_hook(uint8_t* pDest, size_t uiDestSize, uint8_t const* pSrc, size_t uiSegSize, uint32_t uiEscape) 
{
	if (!pDest) {
		return pDest;
	}

	if (!pSrc) {
		return pDest;
	}

	uint8_t* pTempDest = pDest;
	const uint8_t* pTempSrc = pSrc;
	uint8_t* pEndOfDest = &pDest[uiDestSize];

	uint8_t* pEndOfSrc = (uint8_t*)&pSrc[dwRLEDecompressSourceSize];

	if (pDest < pEndOfDest) 
	{
		do 
		{
			if (*pTempSrc == uiEscape) 
			{
				uint8_t ucCurSeg = pTempSrc[1];
				if (ucCurSeg) 
				{
					uint8_t* ucCurDest = pTempDest;
					uint8_t ucCount = 0;
					do 
					{
						++ucCount;

						pDest = (uint8_t*)memcpy(ucCurDest, pTempSrc + 2, uiSegSize);
						
						ucCurDest += uiSegSize;
					} while (ucCurSeg != ucCount);


					pTempDest += uiSegSize * ucCurSeg;
				}
				pTempSrc += 2 + uiSegSize;
			}

			else 
			{
				if (pTempSrc + uiSegSize >= pEndOfSrc)
				{
					return pDest;
				}
				else
				{
					pDest = (uint8_t*)memcpy(pTempDest, pTempSrc, uiSegSize);
					pTempDest += uiSegSize;
					pTempSrc += uiSegSize;
				}
			}
		} while (pEndOfDest > pTempDest);
	}

	dwRLEDecompressSourceSize = 0;

	return pDest;
}

int (*CRealTimeShadowManager__Update)(uintptr_t thiz);
int CRealTimeShadowManager__Update_hook(uintptr_t thiz)
{
	return 0;
}

char* (*RenderQueue__ProcessCommand)(uintptr_t thiz, uintptr_t a2);
char* RenderQueue__ProcessCommand_hook(uintptr_t thiz, uintptr_t a2)
{
	if (!thiz || !a2) return 0;
	return RenderQueue__ProcessCommand(thiz, a2);
}

int (*RwFrameAddChild)(int a1, int a2);
int RwFrameAddChild_hook(int a1, int a2)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x338BD7)
	{
		if(a1 == 0)
			return 0;
	}

	return RwFrameAddChild(a1, a2);
}

int (*CUpsideDownCarCheck__IsCarUpsideDown)(int thiz, uintptr_t a2);
int CUpsideDownCarCheck__IsCarUpsideDown_hook(int thiz, uintptr_t a2)
{
	if(*(uintptr_t*)(a2 + 20))
		return CUpsideDownCarCheck__IsCarUpsideDown(thiz, a2);

	return 0;
}

int (*CAnimManager__UncompressAnimation)(uintptr_t thiz);
int CAnimManager__UncompressAnimation_hook(uintptr_t thiz)
{
	if(!thiz) return 0;
	return CAnimManager__UncompressAnimation(thiz);
}

int (*CGameLogic__IsCoopGameGoingOn)(uintptr_t a1);
int CGameLogic__IsCoopGameGoingOn_hook(uintptr_t a1)
{
    return 0;
}

int (*CAudioEngine__Service)(uintptr_t a1);
int CAudioEngine__Service_hook(uintptr_t a1)
{
	if(!a1) return 0;
	return CAudioEngine__Service(a1);
}

void (*CStreaming__Init2)(uintptr_t thiz);
void CStreaming__Init2_hook(uintptr_t thiz)
{
	CStreaming__Init2(thiz);
	if(pSettings->Get().iTextureLimit != 0)
	{
		/*CPatch::UnFuck(g_libGTASA + 0x5DE734);
		*(uint32_t*)(g_libGTASA + 0x5DE734) = 0x20000000;*/				//250 mb
		if(pSettings->Get().iTextureLimit == 1) memcpy((uint32_t*)(g_libGTASA+0x5DE734), "0x10000000", 10); // CStreaming::ms_memoryAvailable(limit);
												//500 mb
		if(pSettings->Get().iTextureLimit == 2) memcpy((uint32_t*)(g_libGTASA+0x5DE734), "0x20000000", 10); // CStreaming::ms_memoryAvailable(limit);
	}
	else Log("Texture memory limit is disabled");
}

int (*CAEVehicleAudioEntity__GetAccelAndBrake)(int a1, int a2);
int CAEVehicleAudioEntity__GetAccelAndBrake_hook(int a1, int a2)
{
	if(!a1 || !a2) return 0;
	if(!*(uintptr_t *)(a1 + 12)) return 0;
	return CAEVehicleAudioEntity__GetAccelAndBrake(a1, a2);
}

int (*FxEmitterBP_c__Render)(uintptr_t* a1, int a2, int a3, float a4, char a5);
int FxEmitterBP_c__Render_hook(uintptr_t* a1, int a2, int a3, float a4, char a5)
{
    if(!a1 || !a2) return 0;
	if(!a1[3]) return 0;
    return FxEmitterBP_c__Render(a1, a2, a3, a4, a5);
}

int (*CClumpModelInfo__GetFrameFromId)(int a1, int a2);
int CClumpModelInfo__GetFrameFromId_hook(int a1, int a2)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;
	if(!CClumpModelInfo__GetFrameFromId(a1, a2))
	{
		if(dwRetAddr > 0x515707)
		{
			if(dwRetAddr == 0x515708 || dwRetAddr == 0x515730)
				return CClumpModelInfo__GetFrameFromId(a1, a2);
		}
		else if ( dwRetAddr == 0x338698 || dwRetAddr == 0x338B2C )
	    {
	    	return CClumpModelInfo__GetFrameFromId(a1, a2);
	    }

		uintptr_t adress = ((uintptr_t (*)(int, uintptr_t))(g_libGTASA + 0x335CC1))(a1, 2);
		if(!adress)
		{
			for(int i = 3; i < 39; ++i)
			{
				uintptr_t adress = ((uintptr_t (*)(int, uintptr_t))(g_libGTASA + 0x335CC1))(a1, i);
				if(adress) return adress;
			}
		}
		return 0;
	}
	return CClumpModelInfo__GetFrameFromId(a1, a2);
}

uintptr_t (*_GetTexture)(const char* a1, const char* a2);
uintptr_t _GetTexture_hook(const char* a1, const char* a2)
{
	if(!a1) return 0;
	uintptr_t texture = ((uintptr_t(*)(const char*))(g_libGTASA + 0x1BE990 + 1))(a1);
	if (texture == 0) return 0;
	int count = *(int*)(texture + 0x54);
	count++;
	*(int*)(texture + 0x54) = count;
	return texture;
}

int (*CustomPipeRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int CustomPipeRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
    uint16_t size = *(uint16_t *)(resEntry + 26);
    if(size)
    {
        RES_ENTRY_OBJ* arr = (RES_ENTRY_OBJ*)(resEntry + 28);
        uint32_t validFlag = flags & 0x84;
        for(int i = 0; i < size; i++)
        {
            if(!arr[i].validate) break;
            if(validFlag)
            {
                uintptr_t* v4 = *(uintptr_t **)(arr[i].validate);
                if(v4 > (uintptr_t*)0xFFFFFF00) return 0;
                if(v4)
                {
                    if(!*v4) return 0;
                }
            }
        }
    }
    return CustomPipeRenderCB(resEntry, object, type, flags);
}

int (*rxOpenGLDefaultAllInOneRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int rxOpenGLDefaultAllInOneRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
    uint16_t size = *(uint16_t *)(resEntry + 26);
    if(size)
    {
        RES_ENTRY_OBJ* arr = (RES_ENTRY_OBJ*)(resEntry + 28);
        uint32_t validFlag = flags & 0x84;
        for(int i = 0; i < size; i++)
        {
            if(!arr[i].validate) break;
            if(validFlag)
            {
                uintptr_t* v4 = *(uintptr_t **)(arr[i].validate);
                if(v4 > (uintptr_t*)0xFFFFFF00) return 0;
                if(v4)
                {
                    if(!*v4) return 0;
                }
            }
        }
    }
    return rxOpenGLDefaultAllInOneRenderCB(resEntry, object, type, flags);
}

/*void (*CPedDamageResponseCalculator_ComputeDamageResponse)(stPedDamageResponse* thiz, PED_TYPE* pPed, uintptr_t damageResponse, bool bSpeak);
void CPedDamageResponseCalculator_ComputeDamageResponse_hook(stPedDamageResponse* thiz, PED_TYPE* pPed, uintptr_t damageResponse, bool bSpeak)
{
	if(pNetGame && damageResponse)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if(pPlayerPool)
		{
			PLAYERID damagedid = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pPed);
			PLAYERID issuerid = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pPed);

			// self damage like fall damage, drowned, etc
			if(issuerid == INVALID_PLAYER_ID && damagedid == INVALID_PLAYER_ID)
			{
				PLAYERID playerId = pPlayerPool->GetLocalPlayerID();
				pPlayerPool->GetLocalPlayer()->GiveTakeDamage(true, playerId, thiz->fDamage, thiz->dwWeapon, thiz->dwBodyPart);
			}

			// give player damage
			if(issuerid != INVALID_PLAYER_ID && damagedid == INVALID_PLAYER_ID)
				pPlayerPool->GetLocalPlayer()->GiveTakeDamage(false, issuerid, thiz->fDamage, thiz->dwWeapon, thiz->dwBodyPart);

			// player take damage
			else if(issuerid == INVALID_PLAYER_ID && damagedid != INVALID_PLAYER_ID)
				pPlayerPool->GetLocalPlayer()->GiveTakeDamage(true, damagedid, thiz->fDamage, thiz->dwWeapon, thiz->dwBodyPart);
		}
	}

	return CPedDamageResponseCalculator_ComputeDamageResponse(thiz, pPed, damageResponse, bSpeak);
}*/

float m_fWeaponDamages[55] = {
	5.0,   // 0 - Fist
	5.0,   // 1 - Brass knuckles
	5.0,   // 2 - Golf club
	5.0,   // 3 - Nitestick
	5.0,   // 4 - Knife
	5.0,   // 5 - Bat
	5.0,   // 6 - Shovel
	5.0,   // 7 - Pool cue
	5.0,   // 8 - Katana
	5.0,   // 9 - Chainsaw
	5.0,   // 10 - Dildo
	5.0,   // 11 - Dildo 2
	5.0,   // 12 - Vibrator
	5.0,   // 13 - Vibrator 2
	5.0,   // 14 - Flowers
	5.0,   // 15 - Cane
	82.5,  // 16 - Grenade
	0.0,   // 17 - Teargas
	1.0,   // 18 - Molotov
	9.9,   // 19 - Vehicle M4 (custom)
	46.2,  // 20 - Vehicle minigun (custom)
	0.0,   // 21
	8.25,  // 22 - Colt 45
	13.2,  // 23 - Silenced
	46.2,  // 24 - Deagle
	3.3,   // 25 - Shotgun
	3.3,   // 26 - Sawed-off
	4.95,  // 27 - Spas
	6.6,   // 28 - UZI
	8.25,  // 29 - MP5
	9.9,   // 30 - AK47
	9.9,   // 31 - M4
	6.6,   // 32 - Tec9
	24.75, // 33 - Cuntgun
	41.25, // 34 - Sniper
	82.5,  // 35 - Rocket launcher
	82.5,  // 36 - Heatseeker
	1.0,   // 37 - Flamethrower
	46.2,  // 38 - Minigun
	82.5,  // 39 - Satchel
	0.0,   // 40 - Detonator
	0.33,  // 41 - Spraycan
	0.33,  // 42 - Fire extinguisher
	0.0,   // 43 - Camera
	0.0,   // 44 - Night vision
	0.0,   // 45 - Infrared
	0.0,   // 46 - Parachute
	0.0,   // 47 - Fake pistol
	2.64,  // 48 - Pistol whip (custom)
	9.9,   // 49 - Vehicle
	330.0, // 50 - Helicopter blades
	82.5,  // 51 - Explosion
	1.0,   // 52 - Car park (custom)
	1.0,   // 53 - Drowning
	165.0  // 54 - Splat
};

void onDamage(PED_TYPE* issuer, PED_TYPE* damaged)
{
	if (!pNetGame) return;
	PED_TYPE* pPedPlayer = GamePool_FindPlayerPed();
	if (damaged && (pPedPlayer == issuer))
	{
		if (pNetGame->GetPlayerPool()->FindRemotePlayerIDFromGtaPtr((PED_TYPE*)damaged) != INVALID_PLAYER_ID)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			CAMERA_AIM* caAim = pPlayerPool->GetLocalPlayer()->GetPlayerPed()->GetCurrentAim();

			VECTOR aim;
			aim.X = caAim->f1x;
			aim.Y = caAim->f1y;
			aim.Z = caAim->f1z;

			pPlayerPool->GetLocalPlayer()->SendBulletSyncData(pPlayerPool->FindRemotePlayerIDFromGtaPtr((PED_TYPE*)damaged), BULLET_HIT_TYPE_PLAYER, aim);
		}
	}
}

void GiveTakeDamage(PED_TYPE *damaged, PED_TYPE *issuer, int a3, int a4, int bodypart, int hittype, int a7, int a8, int a9, int a10, int a11, int a12, bool a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20, int weaponid)
{
	if(pNetGame)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if(pPlayerPool)
		{
			if (weaponid < 0)
				weaponid = 255; //Suicide
			else if (weaponid == 18)
				weaponid = 37; //Flamethower
			else if (weaponid == 35 || weaponid == 16)
				weaponid = 51; //Explosion
			
			PLAYERID damagedid = pPlayerPool->FindRemotePlayerIDFromGtaPtr((PED_TYPE *)damaged);
			PLAYERID issuerid = pNetGame->GetPlayerPool()->FindRemotePlayerIDFromGtaPtr((PED_TYPE *)issuer);

			CRemotePlayer *pPlayer;
			float fDamaged;

			if (issuerid == INVALID_PLAYER_ID && damagedid == INVALID_PLAYER_ID) //Fall/Drowned or something..
				pPlayer = pPlayerPool->GetAt(pPlayerPool->GetLocalPlayerID());
			else if (issuerid != INVALID_PLAYER_ID && damagedid == INVALID_PLAYER_ID)
				pPlayer = pPlayerPool->GetAt(issuerid);

			if (pPlayer)
			{
				CPlayerPed *pPlayerPed = pPlayer->GetPlayerPed();
				if (pPlayerPed)
				{
					fDamaged = (pPlayer->IsNPC() ? pPlayer->m_fReportedArmour - pPlayerPed->GetArmour() : pPlayerPed->GetArmour() - pPlayer->m_fReportedArmour);
					if (fDamaged == 0.0f)
						fDamaged = (pPlayer->IsNPC() ? pPlayer->m_fReportedHealth - pPlayerPed->GetHealth() : pPlayerPed->GetHealth() - pPlayer->m_fReportedHealth);

					pPlayerPed->SetHealth((pPlayer->IsNPC() ? 100.0f : pPlayer->m_fReportedHealth));
					pPlayerPed->SetArmour((pPlayer->IsNPC() ? 100.0f : pPlayer->m_fReportedArmour));
				}
			}

			// self damage like fall damage, drowned, etc
			if(issuerid == INVALID_PLAYER_ID && damagedid == INVALID_PLAYER_ID)
			{
				PLAYERID playerId = pPlayerPool->GetLocalPlayerID();
				pPlayerPool->GetLocalPlayer()->GiveTakeDamage(true, playerId, m_fWeaponDamages[weaponid], weaponid, bodypart);
			}

			// give player damage
			if(issuerid != INVALID_PLAYER_ID && damagedid == INVALID_PLAYER_ID)
				pPlayerPool->GetLocalPlayer()->GiveTakeDamage(false, issuerid, m_fWeaponDamages[weaponid], weaponid, bodypart);

			// player take damage
			else if(issuerid == INVALID_PLAYER_ID && damagedid != INVALID_PLAYER_ID)
				pPlayerPool->GetLocalPlayer()->GiveTakeDamage(true, damagedid, m_fWeaponDamages[weaponid], weaponid, bodypart);
		}
	}
}

uintptr_t (*ComputeDamageResponse)(uintptr_t, uintptr_t, int, int, int, int, int, int, bool, int, int, int, int, int, int, int, int, int, int, int, int);
uintptr_t ComputeDamageResponse_hook(uintptr_t ped, uintptr_t issuer, int a3, int a4, int bodypart, int hittype, int a7, int a8, int a9, int a10, int a11, int a12, bool a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20, int weaponid)
{
	if (issuer && ped) onDamage((PED_TYPE*)*(uintptr_t*)ped, (PED_TYPE*)issuer);
	GiveTakeDamage((PED_TYPE *)*(uintptr_t *)ped, (PED_TYPE *)issuer, a3, a4, bodypart, hittype, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, weaponid);
	return ComputeDamageResponse(ped, issuer, a3, a4, bodypart, hittype, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, weaponid);
}

uint16_t (*CPed_GetWeaponSkill)(PED_TYPE* pPed);
uint16_t CPed_GetWeaponSkill_hook(PED_TYPE* pPed)
{
	dwCurPlayerActor = (uintptr_t)pPed;
	byteInternalPlayer = *pbyteCurrentPlayer;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);

	if(dwCurPlayerActor && byteCurPlayer != 0 && byteInternalPlayer == 0)
	{
		GameStoreLocalPlayerSkills();
		GameSetRemotePlayerSkills(byteCurPlayer);
		bSetToLocalPlayerSkill = true;
	}
	
	int result = ((int (*)(PED_TYPE *, uint32_t))(g_libGTASA+0x434E88+1))(pPed, pPed->WeaponSlots[pPed->byteCurWeaponSlot].dwType);

	if(bSetToLocalPlayerSkill)
	{
		GameSetLocalPlayerSkills();
		bSetToLocalPlayerSkill = false;
	}

	return result;
}

signed int (*CBulletInfo_AddBullet)(ENTITY_TYPE* pEntity, WEAPON_SLOT_TYPE* pWeapon, VECTOR vec1, VECTOR vec2);
signed int CBulletInfo_AddBullet_hook(ENTITY_TYPE* pEntity, WEAPON_SLOT_TYPE* pWeapon, VECTOR vec1, VECTOR vec2)
{
	vec2.X *= 50.0f;
    vec2.Y *= 50.0f;
    vec2.Z *= 50.0f;
	CBulletInfo_AddBullet(pEntity, pWeapon, vec1, vec2);
	// CBulletInfo::Update
	(( void (*)())(g_libGTASA+0x55E170+1))();
	return 1;
}

uint32_t (*CWeapon_FireInstantHit)(WEAPON_SLOT_TYPE* _this, PED_TYPE* pFiringEntity, VECTOR* vecOrigin, VECTOR* muzzlePos, ENTITY_TYPE* targetEntity, VECTOR *target, VECTOR* originForDriveBy, int arg6, int muzzle);
uint32_t CWeapon_FireInstantHit_hook(WEAPON_SLOT_TYPE* _this, PED_TYPE* pFiringEntity, VECTOR* vecOrigin, VECTOR* muzzlePos, ENTITY_TYPE* targetEntity, VECTOR *target, VECTOR* originForDriveBy, int arg6, int muzzle)
{
	uintptr_t dwRetAddr = 0;
 	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));

 	dwRetAddr -= g_libGTASA;
 	if(	dwRetAddr == 0x569A84 + 1 ||
 		dwRetAddr == 0x569616 + 1 ||
 		dwRetAddr == 0x56978A + 1 ||
 		dwRetAddr == 0x569C06 + 1)
 	{
		PED_TYPE *pLocalPed = pGame->FindPlayerPed()->GetGtaActor();
		if(pLocalPed)
		{
			if(pFiringEntity != pLocalPed)
				return muzzle;

			if(pNetGame)
			{
				CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
				if(pPlayerPool)
				{
					pPlayerPool->ApplyCollisionChecking();
				}
			}

			if(pGame)
			{
				CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
				if(pPlayerPed)
				{
					pPlayerPed->FireInstant();
				}
			}

			if(pNetGame)
			{
				CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
				if(pPlayerPool)
				{
					pPlayerPool->ResetCollisionChecking();
				}
			}

			return muzzle;
		}
 	}

 	return CWeapon_FireInstantHit(_this, pFiringEntity, vecOrigin, muzzlePos, targetEntity, target, originForDriveBy, arg6, muzzle);
}

uint32_t (*CWeapon__FireSniper)(WEAPON_SLOT_TYPE *pWeaponSlot, PED_TYPE *pFiringEntity, ENTITY_TYPE *a3, VECTOR *vecOrigin); 
uint32_t CWeapon__FireSniper_hook(WEAPON_SLOT_TYPE *pWeaponSlot, PED_TYPE *pFiringEntity, ENTITY_TYPE *a3, VECTOR *vecOrigin) 
{ 
    if(GamePool_FindPlayerPed() == pFiringEntity) 
    { 
        if(pGame) 
        { 
            CPlayerPed* pPlayerPed = pGame->FindPlayerPed(); 
            if(pPlayerPed) 
                pPlayerPed->FireInstant(); 
          } 
     } 
    
    return 1; 
}

extern CPlayerPed *g_pCurrentFiredPed;
extern BULLET_DATA *g_pCurrentBulletData;

void SendBulletSync(VECTOR *vecOrigin, VECTOR *vecEnd, VECTOR *vecPos, ENTITY_TYPE **ppEntity)
{
	static BULLET_DATA bulletData;
	memset(&bulletData, 0, sizeof(BULLET_DATA));

	bulletData.vecOrigin.X = vecOrigin->X;
  	bulletData.vecOrigin.Y = vecOrigin->Y;
  	bulletData.vecOrigin.Z = vecOrigin->Z;
  	bulletData.vecPos.X = vecPos->X;
  	bulletData.vecPos.Y = vecPos->Y;
  	bulletData.vecPos.Z = vecPos->Z;

  	if(ppEntity)
	{
  		static ENTITY_TYPE *pEntity;
  		pEntity = *ppEntity;
  		if(pEntity)
		{
  			if(pEntity->mat)
			{
  				if(g_iLagCompensationMode)
				{
  					bulletData.vecOffset.X = vecPos->X - pEntity->mat->pos.X;
		  			bulletData.vecOffset.Y = vecPos->Y - pEntity->mat->pos.Y;
		  			bulletData.vecOffset.Z = vecPos->Z - pEntity->mat->pos.Z;
  				}
				else
				{
  					static MATRIX4X4 mat1;
  					memset(&mat1, 0, sizeof(mat1));

  					static MATRIX4X4 mat2;
		  			memset(&mat2, 0, sizeof(mat2));

		  			RwMatrixOrthoNormalize(&mat2, pEntity->mat);
		  			RwMatrixInvert(&mat1, &mat2);
		  			ProjectMatrix(&bulletData.vecOffset, &mat1, vecPos);
  				}
  			}

  			bulletData.pEntity = pEntity;
  		}
  	}

  	pGame->FindPlayerPed()->ProcessBulletData(&bulletData);
}

uint32_t (*CWorld__ProcessLineOfSight)(VECTOR*,VECTOR*, VECTOR*, PED_TYPE**, bool, bool, bool, bool, bool, bool, bool, bool);
uint32_t CWorld__ProcessLineOfSight_hook(VECTOR* vecOrigin, VECTOR* vecEnd, VECTOR* vecPos, PED_TYPE** ppEntity,
	bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
	uintptr_t dwRetAddr = 0;
 	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));

 	dwRetAddr -= g_libGTASA;
 	if(	dwRetAddr == 0x55E2FE + 1 ||
 		dwRetAddr == 0x5681BA + 1 ||
 		dwRetAddr == 0x567AFC + 1)
 	{
 		ENTITY_TYPE *pEntity = nullptr;
 		MATRIX4X4 *pMatrix = nullptr;
 		static VECTOR vecPosPlusOffset;

 		if(g_iLagCompensationMode != 2)
		{
 			if(g_pCurrentFiredPed != pGame->FindPlayerPed())
			{
 				if(g_pCurrentBulletData)
				{
					if(g_pCurrentBulletData->pEntity)
					{
						if(!pEntity->vtable != g_libGTASA+0x5C7358)
						{
							pMatrix = g_pCurrentBulletData->pEntity->mat;
							if(pMatrix)
							{
								if(g_iLagCompensationMode)
								{
									vecPosPlusOffset.X = pMatrix->pos.X + g_pCurrentBulletData->vecOffset.X;
									vecPosPlusOffset.Y = pMatrix->pos.Y + g_pCurrentBulletData->vecOffset.Y;
									vecPosPlusOffset.Z = pMatrix->pos.Z + g_pCurrentBulletData->vecOffset.Z;
								}
								else
								{
									ProjectMatrix(&vecPosPlusOffset, pMatrix, &g_pCurrentBulletData->vecOffset);
								}

								vecEnd->X = vecPosPlusOffset.X - vecOrigin->X + vecPosPlusOffset.X;
								vecEnd->Y = vecPosPlusOffset.Y - vecOrigin->Y + vecPosPlusOffset.Y;
								vecEnd->Z = vecPosPlusOffset.Z - vecOrigin->Z + vecPosPlusOffset.Z;
							}
						}
					}
 				}
 			}
 		}

 		static uint32_t result = 0;
 		result = CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);

 		if(g_iLagCompensationMode == 2)
		{
 			if(g_pCurrentFiredPed)
			{
 				if(g_pCurrentFiredPed == pGame->FindPlayerPed())
				{
 					SendBulletSync(vecOrigin, vecEnd, vecPos, (ENTITY_TYPE**)ppEntity);
				}
 			}

 			return result;
 		}

 		if(g_pCurrentFiredPed)
		{
 			if(g_pCurrentFiredPed != pGame->FindPlayerPed())
			{
 				if(g_pCurrentBulletData)
				{
 					if(!g_pCurrentBulletData->pEntity)
					{
 						PED_TYPE *pLocalPed = pGame->FindPlayerPed()->GetGtaActor();
 						if(*ppEntity == pLocalPed || (IN_VEHICLE(pLocalPed) &&  *(uintptr_t*)ppEntity == pLocalPed->pVehicle))
						{
 							*ppEntity = nullptr;
 							vecPos->X = 0.0f;
 							vecPos->Y = 0.0f;
 							vecPos->Z = 0.0f;
 							return 0;
 						}
 					}
 				}
 			}
 		}

 		if(g_pCurrentFiredPed)
		{
 			if(g_pCurrentFiredPed == pGame->FindPlayerPed())
			{
 				SendBulletSync(vecOrigin, vecEnd, vecPos, (ENTITY_TYPE **)ppEntity);
			}
 		}

 		return result;
 	}

	return CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);
}

void (*CPlaceable_InitMatrixArray)();
void CPlaceable_InitMatrixArray_hook()
{
	CPatch::CallFunction<void>(g_libGTASA+0x3AB2D8+1, g_libGTASA+0x8B90A8, 10000);
}

int (*_RwTextureDestroy)(RwTexture*);
int _RwTextureDestroy_hook(RwTexture* texture)
{
	if(!texture) return 1;
	return _RwTextureDestroy(texture);
}

uintptr_t (*GetMeshPriority)(RpMesh const*);
uintptr_t GetMeshPriority_hook(RpMesh const* rpMesh)
{	
	if(rpMesh)
	{
		RpMaterial* rpMeshMat = rpMesh->material;
		if(rpMeshMat)
		{
			if(rpMeshMat->texture)
			{
				if(!rpMeshMat->texture->raster) return 0;
			}
		}
	}
	return GetMeshPriority(rpMesh);
}

void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1)
{
  	g_fps->PushThread(gettid());
  	ANDRunThread(a1);
}

#include "..//nv_event.h"
int32_t(*NVEventGetNextEvent_hooked)(NVEvent* ev, int waitMSecs);
int32_t NVEventGetNextEvent_hook(NVEvent* ev, int waitMSecs)
{
	int32_t ret = NVEventGetNextEvent_hooked(ev, waitMSecs);

	if (ret)
	{
		if (ev->m_type == NV_EVENT_MULTITOUCH)
		{
			// process manually
			ev->m_type = (NVEventType)228;
		}

	}

	NVEvent event;
	NVEventGetNextEvent(&event);

	if (event.m_type == NV_EVENT_MULTITOUCH)
	{
		int type = event.m_data.m_multi.m_action & NV_MULTITOUCH_ACTION_MASK;
		int num = (event.m_data.m_multi.m_action & NV_MULTITOUCH_POINTER_MASK) >> NV_MULTITOUCH_POINTER_SHIFT;

		int x1 = event.m_data.m_multi.m_x1;
		int y1 = event.m_data.m_multi.m_y1;

		int x2 = event.m_data.m_multi.m_x2;
		int y2 = event.m_data.m_multi.m_y2;

		int x3 = event.m_data.m_multi.m_x3;
		int y3 = event.m_data.m_multi.m_y3;

		if (type == NV_MULTITOUCH_CANCEL)
		{
			type = NV_MULTITOUCH_UP;
		}

		if ((x1 || y1) || num == 0)
		{
			if (num == 0 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 0, x1, y1); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 0, x1, y1); // AND_TouchEvent
			}
		}

		if ((x2 || y2) || num == 1)
		{
			if (num == 1 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 1, x2, y2); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 1, x2, y2); // AND_TouchEvent
			}
		}
		if ((x3 || y3) || num == 2)
		{
			if (num == 2 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 2, x3, y3); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 2, x3, y3); // AND_TouchEvent
			}
		}
	}

	return ret;
}

void (*NvUtilInit)(void);
void NvUtilInit_hook(void)
{	
	NvUtilInit();
	CPatch::UnFuck(g_libGTASA + 0x5D1608);
	*(char**)(g_libGTASA + 0x5D1608) = CMain::GetBaseStorage();
}

void (*Render2dStuffAfterFade)();
void Render2dStuffAfterFade_hook()
{
	Render2dStuffAfterFade();
	if (pGUI && bGameStarted) pGUI->Render();
}

void (*RenderEffects)();
void RenderEffects_hook()
{	
	// fix render spheres 
	if(pSettings->Get().bnoFX)
	{
		RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)0);
  		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)5);
  		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)6);
  		RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)1);
  		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
  		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);

		CPatch::CallFunction<void>(g_libGTASA + 0x54C330 + 1); 
		CPatch::CallFunction<void>(g_libGTASA + 0x54D738 + 1);
	}
	else
	{
		RenderEffects();
	}
}

void (*CWidget__Update)(WIDGET_TYPE*);
void CWidget__Update_hook(WIDGET_TYPE* pWidget)
{
	if(pNetGame)
	{
		switch (util::widgets::ProcessFixed(pWidget))
		{
		case STATE_NONE: break;
		case STATE_FIXED: return;
		}
	}
	return CWidget__Update(pWidget);
}

void (*CWidget__SetEnabled)(WIDGET_TYPE* pWidget, bool bEnabled);
void CWidget__SetEnabled_hook(WIDGET_TYPE* pWidget, bool bEnabled)
{	
	if(pNetGame)
	{
		switch (util::widgets::ProcessFixed(pWidget))
		{
		case STATE_NONE: break;
		case STATE_FIXED:
			bEnabled = false;
			break;
		}
	} 
	return CWidget__SetEnabled(pWidget, bEnabled);	
}

uintptr_t (*CWidget)(WIDGET_TYPE*, const char*, int, int, int, int);
uintptr_t CWidget_hook(WIDGET_TYPE* thiz, const char* name, int a3, int a4, int a5, int a6)
{	
	//Log("new WIDGET: \"%s\" 0x%X", name, thiz - g_libGTASA);
	util::widgets::SetByName(name, thiz);
	return CWidget(thiz, name, a3, a4, a5, a6);
}
/*
float (*CRadar__LimitRadarPoint)(CVector2D*);
float CRadar__LimitRadarPoint_hook(CVector2D* vector2d)
{
	if ( *(bool*)(g_libGTASA + 0x63E0B4) ) return sqrtf((vector2d->y * vector2d->y) + (vector2d->x * vector2d->x));
	if(pSettings->Get().szRadar) return util::radar::newLimitRadarPoint((float*)vector2d);
	
	CPatch::Write<float>(g_libGTASA + 0x3DED84, 1.5708f);
	return CRadar__LimitRadarPoint(vector2d);
}*/

#include "../jniutil.h"
#include "../keyboard.h"
extern CJavaWrapper *pJavaWrapper;
extern CKeyBoard *pKeyBoard;
void (*CTimer__StartUserPause)(uintptr_t thiz);
void CTimer__StartUserPause_hook(uintptr_t thiz)
{
	CPatch::UnFuck(g_libGTASA+0x8C9BA3);
	*(bool*)(g_libGTASA+0x8C9BA3) = true;

	if(pKeyBoard && pKeyBoard->IsNewKeyboard())
	{
		pKeyBoard->Close();
	}
	
	//if(pSettings->Get().iKeyboard) pKeyBoard->EnableOldKeyboard();
	if(pSettings->Get().iKeyboard != 2) pKeyBoard->EnableOldKeyboard();

	/*if(pNetGame)
	{
		if(pAudioStream && pAudioStream->m_Flags.bPlaying)
			BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 0);
	}*/
}

void (*CTimer__EndUserPause)(uintptr_t thiz);
void CTimer__EndUserPause_hook(uintptr_t thiz)
{
	CPatch::UnFuck(g_libGTASA+0x8C9BA3);
	*(bool*)(g_libGTASA+0x8C9BA3) = false;
	
	if(pSettings->Get().iKeyboard == 2) pKeyBoard->EnableNewKeyboard();
	else pKeyBoard->EnableOldKeyboard();
}

uint32_t (*CAutoMobile__ProcessEntityCollision)(VEHICLE_TYPE *_thizVehicle, VEHICLE_TYPE *_pCollidedVehicle, uintptr_t a3);
uint32_t CAutoMobile__ProcessEntityCollision_hook(VEHICLE_TYPE *_thizVehicle, VEHICLE_TYPE *_pCollidedVehicle, uintptr_t a3)
{
	if(!pNetGame || 
		!pNetGame->m_bDisableRemoteVehicleCollisions || 
		!_thizVehicle || 
		!_pCollidedVehicle ||
		_pCollidedVehicle->entity.nModelIndex < 400 ||
		_pCollidedVehicle->entity.nModelIndex > 611 ||
		!_thizVehicle->pDriver||
		!_pCollidedVehicle->pDriver)
	{
		return CAutoMobile__ProcessEntityCollision(_thizVehicle, _pCollidedVehicle, a3);
	}

	return 0;
}

uint32_t (*CBike__ProcessEntityCollision)(VEHICLE_TYPE *_thizVehicle, VEHICLE_TYPE *_pCollidedVehicle, uintptr_t a3);
uint32_t CBike__ProcessEntityCollision_hook(VEHICLE_TYPE *_thizVehicle, VEHICLE_TYPE *_pCollidedVehicle, uintptr_t a3)
{
	if(!pNetGame || 
		!pNetGame->m_bDisableRemoteVehicleCollisions || 
		!_thizVehicle || 
		!_pCollidedVehicle ||
		_pCollidedVehicle->entity.nModelIndex < 400 ||
		_pCollidedVehicle->entity.nModelIndex > 611 ||
		!_thizVehicle->pDriver||
		!_pCollidedVehicle->pDriver)
	{
		return CBike__ProcessEntityCollision(_thizVehicle, _pCollidedVehicle, a3);
	}

	return 0;
}

uint32_t (*CMonsterTruck__ProcessEntityCollision)(VEHICLE_TYPE *_thizVehicle, VEHICLE_TYPE *_pCollidedVehicle, uintptr_t a3);
uint32_t CMonsterTruck__ProcessEntityCollision_hook(VEHICLE_TYPE *_thizVehicle, VEHICLE_TYPE *_pCollidedVehicle, uintptr_t a3)
{
	if(!pNetGame || 
		!pNetGame->m_bDisableRemoteVehicleCollisions || 
		!_thizVehicle || 
		!_pCollidedVehicle ||
		_pCollidedVehicle->entity.nModelIndex < 400 ||
		_pCollidedVehicle->entity.nModelIndex > 611 ||
		!_thizVehicle->pDriver||
		!_pCollidedVehicle->pDriver)
	{
		return CMonsterTruck__ProcessEntityCollision(_thizVehicle, _pCollidedVehicle, a3);
	}

	return 0;
}

uint32_t (*CTrailer__ProcessEntityCollision)(VEHICLE_TYPE *_thizVehicle, VEHICLE_TYPE *_pCollidedVehicle, uintptr_t a3);
uint32_t CTrailer__ProcessEntityCollision_hook(VEHICLE_TYPE *_thizVehicle, VEHICLE_TYPE *_pCollidedVehicle, uintptr_t a3)
{
	if(!pNetGame || 
		!pNetGame->m_bDisableRemoteVehicleCollisions || 
		!_thizVehicle ||
		!_pCollidedVehicle ||
		_pCollidedVehicle->entity.nModelIndex < 400 ||
		_pCollidedVehicle->entity.nModelIndex > 611 ||
		!_thizVehicle->pDriver||
		!_pCollidedVehicle->pDriver)
	{
		if(!_pCollidedVehicle->pDriver)
		{
			if(pNetGame)
			{
				CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
				if(pVehiclePool)
				{
					VEHICLEID vehicleId = pVehiclePool->FindIDFromGtaPtr(_pCollidedVehicle);
					if(vehicleId != INVALID_VEHICLE_ID)
					{
						CVehicle *pVehicle = pVehiclePool->GetAt(vehicleId);
						if(pVehicle)
						{
							// trailer has attached to any tractor
							if(pVehicle->GetTractor() != NULL)
								return 0;
						}
					}
				}
			}
		}

		return CTrailer__ProcessEntityCollision(_thizVehicle, _pCollidedVehicle, a3);
	}

	return 0;
}

int (*CCollision__BuildCacheOfCameraCollision)(int CColSphere1, int CColSphere2);
int CCollision__BuildCacheOfCameraCollision_hook(int CColSphere1, int CColSphere2)
{
	int result = CCollision__BuildCacheOfCameraCollision(CColSphere1, CColSphere2);
	
	if(*(float*)CColSphere2 <= 3000.0f
		&& *(float*)CColSphere2 >= -3000.0f
		&& *(float*)(CColSphere2+4) <= 3000.0f
		&& *(float*)(CColSphere2+4) >= -3000.0f)
	{
		return result;
	}

	if(pNetGame)
	{
		CObjectPool *pObjectPool = pNetGame->GetObjectPool();
		if(!result)
		{
			if(pObjectPool && pObjectPool->ObjectCollisionProcess(CColSphere1, CColSphere2))
				return true;
		}

		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
		if(!pNetGame->m_bDisableRemoteVehicleCollisions && !result)
		{
			if(pVehiclePool && pVehiclePool->VehicleCollisionProcess(CColSphere1, CColSphere2))
				return true;
		}
	}

	return result;
}

void (*CCam__Process)(uintptr_t);
void CCam__Process_hook(uintptr_t thiz)
{

	/*if (pChatWindow)
	{
		//pChatWindow->AddDebugMessage("time %d %d %d", *(uint32_t*)(g_libGTASA + 0x008B0808 + 116), *(uint32_t*)(g_libGTASA + 0x008B0808 + 120), *(uint32_t*)(g_libGTASA + 0x008B0808 + 124));
		//pChatWindow->AddDebugMessage("camera %d %f %f", *(uint16_t*)(thiz + 14), *(float*)(thiz + 132), *(float*)(thiz + 148)); // 140 - fov, 132 - vertical, 148 - horizontal angle
	}*/

	VECTOR vecSpeed;
	CVehicle* pVeh = nullptr;
	float pOld = *(float*)(g_libGTASA + 0x00608558);
	if (pNetGame && (*(uint16_t*)(thiz + 14) == 18 || *(uint16_t*)(thiz + 14) == 16) && CFirstPersonCamera::IsEnabled())
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
				pVeh = pNetGame->GetVehiclePool()->GetAt(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_CurrentVehicle);
				if (pVeh)
				{
					pVeh->GetMoveSpeedVector(&vecSpeed);
					VECTOR vec;
					vec.X = vecSpeed.X * 6.0f;
					vec.Y = vecSpeed.Y * 6.0f;
					vec.Z = vecSpeed.Z * 6.0f;
					pVeh->SetMoveSpeedVector(vec);
					*(float*)(g_libGTASA + 0x00608558) = 200.0f;
				}
			}
		}
	}

	CCam__Process(thiz);
	if (pVeh)
	{
		pVeh->SetMoveSpeedVector(vecSpeed);
		*(float*)(g_libGTASA + 0x00608558) = pOld;
	}
	if (*(uint16_t*)(thiz + 14) == 4 || *(uint16_t*)(thiz + 14) == 53) // 53 is weapon
	{
		if (pNetGame)
		{
			if (pNetGame->GetPlayerPool())
			{
				if (pNetGame->GetPlayerPool()->GetLocalPlayer())
				{
					CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
					if (pPed)
					{
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 120) = 0xFFFFFFFF;
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 124) = 0xFFFFFFFF;
						*(uint8_t*)(g_libGTASA + 0x008B0808 + 40) = 0;
						CFirstPersonCamera::ProcessCameraOnFoot(thiz, pPed);
					}
				}
			}
		}
	}
	if(* (uint16_t*)(thiz + 14) == 18 || *(uint16_t*)(thiz + 14) == 16) // in car
	{
		if (pNetGame)
		{
			if (pNetGame->GetPlayerPool())
			{
				if (pNetGame->GetPlayerPool()->GetLocalPlayer())
				{
					CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
					if (pPed)
					{
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 120) = 0xFFFFFFFF;
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 124) = 0xFFFFFFFF;
						*(uint8_t*)(g_libGTASA + 0x008B0808 + 40) = 0;
						CFirstPersonCamera::ProcessCameraInVeh(thiz, pPed, pVeh);
					}
				}
			}
		}
	}
}

int (*CCollision__CheckCameraCollisionVehicles)(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
int CCollision__CheckCameraCollisionVehicles_hook(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
	if(!pNetGame || !pNetGame->m_bDisableRemoteVehicleCollisions)
		return CCollision__CheckCameraCollisionVehicles(a1, a2, a3, a4, a5, a6, a7);
		
	return 0;
}

int (*CWorld__CameraToIgnoreThisObject)(uintptr_t a1);
int CWorld__CameraToIgnoreThisObject_hook(uintptr_t a1)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x29CB2A + 1)
	{
		if(a1 && *(uint32_t*)(a1+0xFC))
			return 1;
	}

	return CWorld__CameraToIgnoreThisObject(a1);
}

VECTOR& (*FindPlayerSpeed)(int a1);
VECTOR& FindPlayerSpeed_hook(int a1)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x3DF20E + 1)
	{
		if(pNetGame)
		{
			CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed &&
				pPlayerPed->IsInVehicle() &&
				pPlayerPed->IsAPassenger())
			{
				VECTOR vec = {0.1f, 0.1f, 0.1f};
				return vec;
			}
		}
	}

	return FindPlayerSpeed(a1);
}

uint32_t (*CVehicle__GetVehicleLightsStatus)(VEHICLE_TYPE *_pVehicle);
uint32_t CVehicle__GetVehicleLightsStatus_hook(VEHICLE_TYPE *_pVehicle)
{
	if(pNetGame && pNetGame->m_bManualVehicleEngineAndLight)
	{
		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
		if(pVehiclePool)
		{
			VEHICLEID vehicleId = pVehiclePool->FindIDFromGtaPtr(_pVehicle);
			if(vehicleId != INVALID_VEHICLE_ID)
			{
				CVehicle *pVehicle = pVehiclePool->GetAt(vehicleId);
				if(pVehicle)
				{
					if(pVehicle->GetLightsState() == 0 || pVehicle->GetLightsState() == 1)
						return pVehicle->GetLightsState();
				}
			}
		}
	}

	return CVehicle__GetVehicleLightsStatus(_pVehicle);
}

void (*CVehicle__DoHeadLightBeam)(VEHICLE_TYPE *vehicle, int v2, MATRIX4X4 *matrix, uint8_t v4);
void CVehicle__DoHeadLightBeam_hook(VEHICLE_TYPE *vehicle, int v2, MATRIX4X4 *matrix, uint8_t v4)
{
    uint8_t r, g, b = -1;
    if(pNetGame)
    {
        CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
        if(pVehiclePool)
        {
            VEHICLEID vehicleId = pVehiclePool->FindIDFromGtaPtr(vehicle);
            if(vehicleId != INVALID_VEHICLE_ID)
            {
                CVehicle* pVehicle = pVehiclePool->GetAt(vehicleId);
                if(pVehicle)
                    pVehicle->ProcessHeadlightsColor(&r, &g, &b);
            }
        }
    }

    *(uint8_t*)(g_libGTASA + 0x9BAA70) = r;
    *(uint8_t*)(g_libGTASA + 0x9BAA71) = g;
    *(uint8_t*)(g_libGTASA + 0x9BAA72) = b;
    *(uint8_t*)(g_libGTASA + 0x9BAA94) = r;
    *(uint8_t*)(g_libGTASA + 0x9BAA95) = g;
    *(uint8_t*)(g_libGTASA + 0x9BAA96) = b;
    *(uint8_t*)(g_libGTASA + 0x9BAB00) = r;
    *(uint8_t*)(g_libGTASA + 0x9BAB01) = g;
    *(uint8_t*)(g_libGTASA + 0x9BAB02) = b;

    CVehicle__DoHeadLightBeam(vehicle, v2, matrix, v4);
}

void (*CShadows__StoreCarLightShadow)(VEHICLE_TYPE *vehicle, int iId, RwTexture *pTexture, VECTOR *vecPosn, float fFrontX, float fFrontY, float fSideX, float fSideY, uint8_t byteRed, uint8_t byteGreen, uint8_t byteBlue, float fMaxViewAngle);
void CShadows__StoreCarLightShadow_hook(VEHICLE_TYPE *vehicle, int iId, RwTexture *pTexture, VECTOR *vecPosn, float fFrontX, float fFrontY, float fSideX, float fSideY, uint8_t byteRed, uint8_t byteGreen, uint8_t byteBlue, float fMaxViewAngle)
{
	uint8_t byteCarLightShadowRed = byteRed;
	uint8_t byteCarLightShadowGreen = byteGreen;
	uint8_t byteCarLightShadowBlue = byteBlue;
    if(pNetGame)
    {
        CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
        if(pVehiclePool)
        {
            VEHICLEID vehicleId = pVehiclePool->FindIDFromGtaPtr(vehicle);
			if(vehicleId != INVALID_VEHICLE_ID)
			{
				CVehicle* pVehicle = pVehiclePool->GetAt(vehicleId);
				if(pVehicle)
					pVehicle->ProcessHeadlightsColor(&byteCarLightShadowRed, &byteCarLightShadowGreen, &byteCarLightShadowBlue);
			}
        }
    }

  	CShadows__StoreCarLightShadow(vehicle, iId, pTexture, vecPosn, fFrontX, fFrontY, fSideX, fSideY, byteCarLightShadowRed, byteCarLightShadowGreen, byteCarLightShadowBlue, fMaxViewAngle);
}

int (*CPlayerInfo__FindObjectToSteal)(unsigned int a1, unsigned int a2);
int CPlayerInfo__FindObjectToSteal_hook(unsigned int a1, unsigned int a2)
{
	return 0;
}

int (*emu_ArraysGetID)(unsigned int a1);
int emu_ArraysGetID_hook(unsigned int a1)
{
	if(!a1 || !*(uint32_t*)(a1+36)) return 0;
	return emu_ArraysGetID(a1);
}

int (*CPed__GetBonePosition)(int a1, int *a2, int a3, int a4);
int CPed__GetBonePosition_hook(int a1, int *a2, int a3, int a4)
{
	if(!a1) return 0;
	return CPed__GetBonePosition(a1, a2, a3, a4);
}

int (*RpLightDestroy)(int result);
int RpLightDestroy_hook(int result)
{
	if(!result) return 0;
	return RpLightDestroy(result);
}

int (*CGame__Shutdown)(unsigned int a1, int a2, unsigned int a3);
int CGame__Shutdown_hook(unsigned int a1, int a2, unsigned int a3)
{
	Log("Exiting game...");
	CPatch::NOP(g_libGTASA + 0x341FCC, 2);
	CPatch::NOP(g_libGTASA + 0x46389E, 2);

	CMain::Terminate();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	return CGame__Shutdown(a1, a2, a3);
}

int (*OS_FileRead)(void* a1, void* a2, int a3);
int OS_FileRead_hook(void* a1, void* a2, int a3)
{
	uintptr_t calledFrom = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (calledFrom));
	calledFrom -= g_libGTASA;

	if (!a3) {
		return 0;
	}

	if (calledFrom == 0x1BDD34 + 1)
	{
		int retn = OS_FileRead(a1, a2, a3);
		dwRLEDecompressSourceSize = *(uint32_t*)a2; // save value for RLEDecompress
		return retn;
	}

	return OS_FileRead(a1, a2, a3);
}

void (*CPed__PlayFootSteps)(uintptr_t pPed, int a2, int a3, int a4);
void CPed__PlayFootSteps_hook(uintptr_t pPed, int a2, int a3, int a4)
{
	if(!pPed) {
		return;
	}

	// RpAnimBlendClumpGetFirstAssociation
	uintptr_t pClump = *(uintptr_t*)(pPed + 0x18);

	if(pClump) 
	{
		uintptr_t pClumpAssociation = ((uintptr_t (*)(uintptr_t, int, int, int))(g_libGTASA + 0x0033D970 + 1))(pClump, a2, a3, a4); // TODO: OFFSETS

		if(pClumpAssociation) {
			CPed__PlayFootSteps(pPed, a2, a3, a4);
		}
	}
}


float (*CRadar__LimitRadarPoint)(float* a1);
float CRadar__LimitRadarPoint_hook(float* a1)
{
	if (*(uint8_t*)(g_libGTASA + 0x0063E0B4))
	{
		return sqrtf((float)(a1[1] * a1[1]) + (float)(*a1 * *a1));
	}

	if (!CRadarRect::IsEnabled())
	{
		return CRadar__LimitRadarPoint(a1);
	}
	float value = CRadarRect::CRadar__LimitRadarPoint_hook(a1);

	return value;
}

void (*CSprite2d__Draw)(CSprite2d* a1, CRect* a2, CRGBA* a3);
void CSprite2d__Draw_hook(CSprite2d* a1, CRect* a2, CRGBA* a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (!pGUI)
	{
		return CSprite2d__Draw(a1, a2, a3);
	}

	if (dwRetAddr == 0x003D3796 + 1 || dwRetAddr == 0x003D376C + 1 || dwRetAddr == 0x003D373E + 1 || dwRetAddr == 0x003D3710 + 1)
	{
		if (CRadarRect::m_pDiscTexture == nullptr)
		{
			CRadarRect::m_pDiscTexture = a1->m_pTexture;
		}
		if (CRadarRect::IsEnabled() && CRadarRect::m_pRectTexture) {
			a1->m_pTexture = CRadarRect::m_pRectTexture;
		}
		else a1->m_pTexture = CRadarRect::m_pDiscTexture;

		if(pNetGame->GetGameState() == GAMESTATE_CONNECTED && !pKeyBoard->IsOpen() && !pDialogWindow->m_bIsActive) // Фиксит показ обводки при подключении, диалогах
		{
			if(pSettings->Get().iRadarAlpha)
			{
				// Hud прозрачный цвет
				a3->a = 42;
				a3->b = 187;
				a3->g = 155;
				a3->r = 1;
			}
			else {
				a3->a = 255;
				a3->b = 255;
				a3->g = 255;
				a3->r = 255;
			}
		}
		else 
		{
			a3->a = 42;
			a3->b = 187;
			a3->g = 155;
			a3->r = 1;
		}

		float* thiz = (float*) * (uintptr_t*)(g_libGTASA + 0x6580C8);
		if (thiz)
		{
			thiz[3] = 50.0f;
			thiz[4] = 70.0f;
			thiz[5] = 45.0f;
			thiz[6] = 45.0f;
		}
		
	}

	return CSprite2d__Draw(a1, a2, a3);
}

void InstallSpecialHooks()
{	
	g_fps = new CFPSFix();
	
	CPatch::InlineHook(g_libGTASA, 0x23BEDC, &OS_FileRead_hook, &OS_FileRead); 
    //CPatch::MethodHook(g_libGTASA, 0x5DDC60, &Init_hook);
	//CPatch::InlineHook(g_libGTASA, 0x4D3864, &CText_Get_hook, &CText_Get)
	CPatch::InlineHook(g_libGTASA, 0x23768C, &ANDRunThread_hook, &ANDRunThread);
	CPatch::InlineHook(g_libGTASA, 0x23ACC4, &NVEventGetNextEvent_hook, &NVEventGetNextEvent_hooked);
	CPatch::InlineHook(g_libGTASA, 0x241D94, &NvUtilInit_hook, &NvUtilInit);
	CPatch::InlineHook(g_libGTASA, 0x40C530, &InitialiseRenderWare_hook, &InitialiseRenderWare);
	CPatch::InlineHook(g_libGTASA, 0x25E660, &CMainMenu__Update_hook, &CMainMenu__Update);
	CPatch::InlineHook(g_libGTASA, 0x269974, &MenuItem_add_hook, &MenuItem_add);
	CPatch::InlineHook(g_libGTASA, 0x25AFF4, &MobileMenu__Render_hook, &MobileMenu__Render);
	CPatch::InlineHook(g_libGTASA, 0x23BB84, &OS_FileOpen_hook, &OS_FileOpen); // TODO: TEST
	//CPatch::InlineHook(g_libGTASA, 0x23BB84, &OS_FileOpen_hook, &OS_FileOpen); // TODO: TEST
	//CPatch::InlineHook(g_libGTASA, 0x23B3DC, &NvFOpen_hook, &NvFOpen);
	CPatch::InlineHook(g_libGTASA, 0x4042A8, &CStreaming__Init2_hook, &CStreaming__Init2);
	CPatch::InlineHook(g_libGTASA, 0x3AF1A0, &CPools_Initialise_hook, &CPools_Initialise);

	// fixes
	CPatch::InlineHook(g_libGTASA, 0x1B1808, &_RwTextureDestroy_hook, &_RwTextureDestroy);
	CPatch::InlineHook(g_libGTASA, 0x50C5F8, &SetCompAlphaCB_hook, &SetCompAlphaCB);
	CPatch::InlineHook(g_libGTASA, 0x1BDC3C, &CTextureDatabaseRuntime__GetEntry_hook, &CTextureDatabaseRuntime__GetEntry);
	CPatch::InlineHook(g_libGTASA, 0x1B9D74, &_rwFreeListFreeReal_hook, &_rwFreeListFreeReal);
	CPatch::InlineHook(g_libGTASA, 0x41EAB4, &CPlayerPedDataSaveStructure__Construct_hook, &CPlayerPedDataSaveStructure__Construct);
	CPatch::InlineHook(g_libGTASA, 0x33AD78, &CAnimBlendNode__FindKeyFrame_hook, &CAnimBlendNode__FindKeyFrame);
	CPatch::InlineHook(g_libGTASA, 0x1BC314, &RLEDecompress_hook, &RLEDecompress);
	CPatch::InlineHook(g_libGTASA, 0x541AC4, &CRealTimeShadowManager__Update_hook, &CRealTimeShadowManager__Update);
	CPatch::InlineHook(g_libGTASA, 0x1A8530, &RenderQueue__ProcessCommand_hook, &RenderQueue__ProcessCommand);
	CPatch::InlineHook(g_libGTASA, 0x1AECC0, &RwFrameAddChild_hook, &RwFrameAddChild);
	CPatch::InlineHook(g_libGTASA, 0x2DFD30, &CUpsideDownCarCheck__IsCarUpsideDown_hook, &CUpsideDownCarCheck__IsCarUpsideDown);
	CPatch::InlineHook(g_libGTASA, 0x33DA5C, &CAnimManager__UncompressAnimation_hook, &CAnimManager__UncompressAnimation);
	CPatch::InlineHook(g_libGTASA, 0x368850, &CAudioEngine__Service_hook, &CAudioEngine__Service);
	CPatch::InlineHook(g_libGTASA, 0x35AC44, &CAEVehicleAudioEntity__GetAccelAndBrake_hook, &CAEVehicleAudioEntity__GetAccelAndBrake);
	CPatch::InlineHook(g_libGTASA, 0x31B164, &FxEmitterBP_c__Render_hook, &FxEmitterBP_c__Render);
	CPatch::InlineHook(g_libGTASA, 0x335CC0, &CClumpModelInfo__GetFrameFromId_hook, &CClumpModelInfo__GetFrameFromId);
	CPatch::InlineHook(g_libGTASA, 0x258910, &_GetTexture_hook, &_GetTexture);
	CPatch::InlineHook(g_libGTASA, 0x28AAAC, &CustomPipeRenderCB_hook, &CustomPipeRenderCB);
	CPatch::InlineHook(g_libGTASA, 0x1EEC90, &rxOpenGLDefaultAllInOneRenderCB_hook, &rxOpenGLDefaultAllInOneRenderCB);
	CPatch::InlineHook(g_libGTASA, 0x3ABB08, &CPlaceable_InitMatrixArray_hook, &CPlaceable_InitMatrixArray);
	CPatch::InlineHook(g_libGTASA, 0x531118, &CCustomRoadsignMgr__RenderRoadsignAtomic_hook, &CCustomRoadsignMgr__RenderRoadsignAtomic);
	CPatch::InlineHook(g_libGTASA, 0x1E4AE4, &GetMeshPriority_hook, &GetMeshPriority);
	// new
	CPatch::InlineHook(g_libGTASA, 0x3AC114, &CPlayerInfo__FindObjectToSteal_hook, &CPlayerInfo__FindObjectToSteal);
	CPatch::InlineHook(g_libGTASA, 0x194B20, &emu_ArraysGetID_hook, &emu_ArraysGetID);
	CPatch::InlineHook(g_libGTASA, 0x436590, &CPed__GetBonePosition_hook, &CPed__GetBonePosition);
	CPatch::InlineHook(g_libGTASA, 0x1E3810, &RpLightDestroy_hook, &RpLightDestroy);
	CPatch::InlineHook(g_libGTASA, 0x2C3868, &CGameLogic__IsCoopGameGoingOn_hook, &CGameLogic__IsCoopGameGoingOn);
	CPatch::InlineHook(g_libGTASA, 0x5E57F0, &CPed__PlayFootSteps_hook, &CPed__PlayFootSteps);
	
}

void InstallHooks()
{	
	CPatch::InlineHook(g_libGTASA, 0x389D74, &CCam__Process_hook, &CCam__Process);
	CPatch::InlineHook(g_libGTASA, 0x2749B0, &CWidget_hook, &CWidget);
	CPatch::InlineHook(g_libGTASA, 0x273338, &CWidget__Update_hook, &CWidget__Update);
	CPatch::InlineHook(g_libGTASA, 0x274178, &CWidget__SetEnabled_hook, &CWidget__SetEnabled);
	CPatch::InlineHook(g_libGTASA, 0x3DA86C, &CRadar__LimitRadarPoint_hook, &CRadar__LimitRadarPoint);
	CPatch::InlineHook(g_libGTASA, 0x3986CC, &CGame__Process_hook, &CGame__Process);
	CPatch::InlineHook(g_libGTASA, 0x39B098, &Render2dStuffAfterFade_hook, &Render2dStuffAfterFade);
	CPatch::InlineHook(g_libGTASA, 0x39AE28, &RenderEffects_hook, &RenderEffects);
	CPatch::InlineHook(g_libGTASA, 0x239D5C, &TouchEvent_hook, &TouchEvent);
	CPatch::InlineHook(g_libGTASA, 0x3EF518, &CObject__Render_hook, &CObject__Render);
	CPatch::InlineHook(g_libGTASA, 0x391E20, &CEntity_Render_hook, &CEntity_Render);
	//CPatch::InlineHook(g_libGTASA, 0x53021C, &CCustomCarPlateMgr__CreatePlateTexture_hook, &CCustomCarPlateMgr__CreatePlateTexture); // кастомные номера на авто
	//CPatch::InlineHook(g_libGTASA, 0x3C14B0, &CWorld__Add_hook, &CWorld__Add);
	//CPatch::InlineHook(g_libGTASA, 0x3C1500, &CWorld__Remove_hook, &CWorld__Remove);
	CPatch::InlineHook(g_libGTASA, 0x3D4EAC, &CHud_DrawVitalStats_hook, &CHud_DrawVitalStats);
	CPatch::InlineHook(g_libGTASA, 0x3C1BF8, &CWorld__ProcessPedsAfterPreRender_hook, &CWorld__ProcessPedsAfterPreRender);
	CPatch::InlineHook(g_libGTASA, 0x336268, &CModelInfo_AddAtomicModel_hook, &CModelInfo_AddAtomicModel);

	CPatch::InlineHook(g_libGTASA, 0x46D6AC, &CTaskSimpleUseGun__SetPedPosition_hook, &CTaskSimpleUseGun__SetPedPosition);
	//CPatch::InlineHook(g_libGTASA, 0x327528, &CPedDamageResponseCalculator_ComputeDamageResponse_hook, &CPedDamageResponseCalculator_ComputeDamageResponse);
	CPatch::InlineHook(g_libGTASA, 0x327528, &ComputeDamageResponse_hook, &ComputeDamageResponse);
	CPatch::InlineHook(g_libGTASA, 0x434F24, &CPed_GetWeaponSkill_hook, &CPed_GetWeaponSkill);
	CPatch::InlineHook(g_libGTASA, 0x567964, &CWeapon_FireInstantHit_hook, &CWeapon_FireInstantHit);
	CPatch::InlineHook(g_libGTASA, 0x3C70C0, &CWorld__ProcessLineOfSight_hook, &CWorld__ProcessLineOfSight);
	CPatch::InlineHook(g_libGTASA, 0x56668C, &CWeapon__FireSniper_hook, &CWeapon__FireSniper);
	CPatch::InlineHook(g_libGTASA, 0x55E090, &CBulletInfo_AddBullet_hook, &CBulletInfo_AddBullet);

	CPatch::InlineHook(g_libGTASA, 0x3D7CA8, &CLoadingScreen_DisplayPCScreen_hook, &CLoadingScreen_DisplayPCScreen);
	CPatch::InlineHook(g_libGTASA, 0x28E83C, &CStreaming_InitImageList_hook, &CStreaming_InitImageList);
	CPatch::InlineHook(g_libGTASA, 0x336690, &CModelInfo_AddPedModel_hook, &CModelInfo_AddPedModel);
	CPatch::InlineHook(g_libGTASA, 0x3DBA88, &CRadar__GetRadarTraceColor_hook, &CRadar__GetRadarTraceColor);
	CPatch::InlineHook(g_libGTASA, 0x3DAF84, &CRadar__SetCoordBlip_hook, &CRadar__SetCoordBlip);
	CPatch::InlineHook(g_libGTASA, 0x3DE9A8, &CRadar__DrawRadarGangOverlay_hook, &CRadar__DrawRadarGangOverlay);
	CPatch::InlineHook(g_libGTASA, 0x482E60, &CTaskComplexEnterCarAsDriver_hook, &CTaskComplexEnterCarAsDriver);
	CPatch::InlineHook(g_libGTASA, 0x4833CC, &CTaskComplexLeaveCar_hook, &CTaskComplexLeaveCar);
	
	// camera collisions processing
	CPatch::InlineHook(g_libGTASA, 0x29CBB0, &CCollision__BuildCacheOfCameraCollision_hook, &CCollision__BuildCacheOfCameraCollision);
	CPatch::InlineHook(g_libGTASA, 0x29C924, &CCollision__CheckCameraCollisionVehicles_hook, &CCollision__CheckCameraCollisionVehicles);
	CPatch::InlineHook(g_libGTASA, 0x3C6860, &CWorld__CameraToIgnoreThisObject_hook, &CWorld__CameraToIgnoreThisObject);
	
	// passenger radar speed fix
	CPatch::InlineHook(g_libGTASA, 0x3AC45C, &FindPlayerSpeed_hook, &FindPlayerSpeed);
	
	// vehicle light processing
	CPatch::InlineHook(g_libGTASA, 0x5189C4, &CVehicle__GetVehicleLightsStatus_hook, &CVehicle__GetVehicleLightsStatus);
	
	// custom vehicle headlights
	CPatch::InlineHook(g_libGTASA, 0x518EC4, &CVehicle__DoHeadLightBeam_hook, &CVehicle__DoHeadLightBeam);
	CPatch::InlineHook(g_libGTASA, 0x5466EC, &CShadows__StoreCarLightShadow_hook, &CShadows__StoreCarLightShadow);
	
	// pause
	CPatch::InlineHook(g_libGTASA, 0x3BF784, &CTimer__StartUserPause_hook, &CTimer__StartUserPause);
	CPatch::InlineHook(g_libGTASA, 0x3BF7A0, &CTimer__EndUserPause_hook, &CTimer__EndUserPause);

	CPatch::InlineHook(g_libGTASA, 0x4D4A6C, &CAutoMobile__ProcessEntityCollision_hook, &CAutoMobile__ProcessEntityCollision);
	CPatch::InlineHook(g_libGTASA, 0x4E9868, &CBike__ProcessEntityCollision_hook, &CBike__ProcessEntityCollision);
	CPatch::InlineHook(g_libGTASA, 0x500328, &CMonsterTruck__ProcessEntityCollision_hook, &CMonsterTruck__ProcessEntityCollision);
	CPatch::InlineHook(g_libGTASA, 0x5076C0, &CTrailer__ProcessEntityCollision_hook, &CTrailer__ProcessEntityCollision);
	CPatch::InlineHook(g_libGTASA, 0x398334, &CGame__Shutdown_hook, &CGame__Shutdown);
	CPatch::InlineHook(g_libGTASA, 0x55265C, &CSprite2d__Draw_hook, &CSprite2d__Draw);

    CPatch::CodeInject(g_libGTASA + 0x2D99F4, (uintptr_t)PickupPickUp_hook, 1);
	HookCPad();
}
