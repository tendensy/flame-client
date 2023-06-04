#include "../main.h"
#include "../util/patch.h"
#include "../game/common.h"
#include "../settings.h"
extern CSettings* pSettings;

char* PLAYERS_REALLOC = nullptr;

void ApplyFPSPatch(uint8_t fps)
{
	__android_log_print(ANDROID_LOG_INFO, "AXL", "Applying global patches.. (level 0)");
	char fpsch[15];
	sprintf(fpsch, "\\x%x", 144);
	CPatch::WriteMemory(g_libGTASA + 0x463FE8, fpsch, 1);
	CPatch::WriteMemory(g_libGTASA + 0x56C1F6, fpsch, 1);
	CPatch::WriteMemory(g_libGTASA + 0x56C126, fpsch, 1);
	CPatch::WriteMemory(g_libGTASA + 0x95B074, fpsch, 1);
	CPatch::WriteMemory(g_libGTASA + 0x56C1A2, fpsch, 1);
}

void ExitSocialClub()
{
	uintptr_t g_libSCAnd = FindLibrary("libSCAnd.so");
	CPatch::WriteMemory(g_libSCAnd + 0x1E16DC, cryptor::create("com/rockstargames/hal/andViewManager", 37).decrypt(), 37);
	CPatch::WriteMemory(g_libSCAnd + 0x1E1738, cryptor::create("staticExitSocialClub", 21).decrypt(), 21);
	CPatch::WriteMemory(g_libSCAnd + 0x1E080C, cryptor::create("()V", 4).decrypt(), 4);
}

void ApplyETCpatches()
{		
	// Settings
	CPatch::NOP(g_libGTASA + 0x266460, 2); // Game - TrafficMode
	CPatch::NOP(g_libGTASA + 0x266496, 2); // Game - AimMode
                  CPatch::NOP(g_libGTASA + 0x261A50, 2); // Language
                  CPatch::NOP(g_libGTASA + 0x2661DA, 2); // Display - Shadows
                  CPatch::NOP(g_libGTASA + 0x2665EE, 2); // Game - SocialClub

	CPatch::RET(g_libGTASA + 0x541AA8); // CRealTimeShadowManager::ReturnRealTimeShadow from ~CPhysical
	CPatch::RET(g_libGTASA + 0x541AC4); // CRealTimeShadowManager::Update
	CPatch::RET(g_libGTASA + 0x543B04); // CShadows::RenderStaticShadows
	CPatch::RET(g_libGTASA + 0x5471F4); // CShadows::RenderStoredShadows
	CPatch::RET(g_libGTASA + 0x545C04); // CShadows::StoreStaticShadow

  	CPatch::RET(g_libGTASA + 0x3474E0);	// CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire
	CPatch::RET(g_libGTASA + 0x284BB8); // CWidgetRegionSteeringSelection::Draw
  	CPatch::RET(g_libGTASA + 0x3BF8B4);	// CPlaceName::Process
  	CPatch::RET(g_libGTASA + 0x4BDB18);	// CTaskSimplePlayerOnFoot::PlayIdleAnimations
  	CPatch::RET(g_libGTASA + 0x494FE4);	// CCarEnterExit::SetPedInCarDirect
	CPatch::RET(g_libGTASA + 0x3DA500); // CRadar::DrawLgegend
	CPatch::RET(g_libGTASA + 0x3DBB30); // CRadar::AddBlipToLegendList
	CPatch::RET(g_libGTASA + 0x5366D0); // CGlass::Render
	CPatch::RET(g_libGTASA + 0x5286EC); // CBirds::Render
	CPatch::RET(g_libGTASA + 0x4DD00C); // CAutomobile::CustomCarPlate_BeforeRenderingStart
	CPatch::RET(g_libGTASA + 0x4DD038); // CAutomobile::CustomCarPlate_AfterRenderingStop
	CPatch::RET(g_libGTASA + 0x36E77C); // CCamera::CamShake
	CPatch::RET(g_libGTASA + 0x392A98); // CEntity::PreRenderForGlassWindow
	CPatch::RET(g_libGTASA + 0x315310); // CStuntJumpManager::Update
	CPatch::RET(g_libGTASA + 0x54DCF4); // CMirrors::RenderReflBuffer
	CPatch::RET(g_libGTASA + 0x3B67F8); // CRopes::Update
	CPatch::RET(g_libGTASA + 0x4FDC78); // CHeli::UpdateHelis
	CPatch::RET(g_libGTASA + 0x3D541C); // CHud::DrawVehicleName
	CPatch::RET(g_libGTASA + 0x3D62FC); // CHud::DrawBustedWastedMessage
	CPatch::RET(g_libGTASA + 0x3D4244); // CHud::SetHelpMessage
	CPatch::RET(g_libGTASA + 0x3D42A8); // CHud::SetHelpMessageStatUpdate
	CPatch::RET(g_libGTASA + 0x2BCD0C); // CCheat::ProcessCheatMenu
	CPatch::RET(g_libGTASA + 0x2BCDB4); // CCheat::ProcessCheats
	CPatch::RET(g_libGTASA + 0x2BC08C); // CCheat::AddToCheatString
	CPatch::RET(g_libGTASA + 0x2BC24C); // CCheat::DoCheats 
	CPatch::RET(g_libGTASA + 0x2BA710); // CCheat::WeaponCheat1
	CPatch::RET(g_libGTASA + 0x2BA92C); // CCheat::WeaponCheat2
	CPatch::RET(g_libGTASA + 0x2BAB20); // CCheat::WeaponCheat3
	CPatch::RET(g_libGTASA + 0x2BA68C); // CCheat::WeaponCheat4
	CPatch::RET(g_libGTASA + 0x2C9EEC); // CGarages::TriggerMessage

	CPatch::RET(g_libGTASA + 0x2DC8E0); // CVehicleRecording::Load
	CPatch::RET(g_libGTASA + 0x463870);	// C_PcSave::SaveSlot
	CPatch::RET(g_libGTASA + 0x2DE734); // CRoadBlocks::GenerateRoadBlocks
                  CPatch::RET(g_libGTASA + 0x4F90AC); // CTheCarGenerators::Process
                  CPatch::RET(g_libGTASA + 0x45F1A4); // CPopulation::AddPed
  	CPatch::RET(g_libGTASA + 0x2E82CC); // CCarCtrl::GenerateRandomCars
                  CPatch::RET(g_libGTASA + 0x504DB8); // CPlane::DoPlaneGenerationAndRemoval
                  CPatch::RET(g_libGTASA + 0x401BAC); // CFileLoader::LoadPickup
                  CPatch::RET(g_libGTASA + 0x2C1CB0); // CEntryExit::GenerateAmbientPeds
	CPatch::RET(g_libGTASA + 0x2B055C); // CCarCtrl::GenerateOneEmergencyServicesCar
	CPatch::RET(g_libGTASA + 0x3E17F0); // Interior_c::AddPickups
	CPatch::RET(g_libGTASA + 0x3E42E0); // InteriorGroup_c::Exit
	CPatch::RET(g_libGTASA + 0x3E4490); // InteriorGroup_c::Setup
	CPatch::RET(g_libGTASA + 0x3E3F38); // InteriorGroup_c::SetupPeds

	CPatch::NOP(g_libGTASA + 0x408AAA, 2); 	// RpWorldAddLight from ~LightsCreate
	CPatch::NOP(g_libGTASA + 0x454950, 17); // CAnimManager::RemoveAnimBlockRef from CPlayerPed::ProcessAnimGroups
	CPatch::NOP(g_libGTASA + 0x56C150, 2); 	// CAudioEngine::StartLoadingTune from DoGameState
	CPatch::NOP(g_libGTASA + 0x45477E, 5);	// CPlayerPed::ReApplyMoveAnims from CPlayerPed::ProcessAnimGroups
	CPatch::NOP(g_libGTASA + 0x3DCA90, 2);  // CSprite2d::Draw from CRadar::DrawCoordBlip
                  CPatch::NOP(g_libGTASA + 0x3DD4A4, 2);	// CSprite2d::Draw from CRadar::DrawEntityBlip
                  CPatch::NOP(g_libGTASA + 0x434D94, 6); 	// CPlayerPed::GetPlayerInfoForThisPlayerPed from CPed::RemoveWeaponWhenEnteringVehicle
                  CPatch::NOP(g_libGTASA + 0x4E31A6, 2); 	// CAEVehicleAudioEntity::Service from CAutomobile::ProcessControl
                  CPatch::NOP(g_libGTASA + 0x4EE7D2, 2); 	// CAEVehicleAudioEntity::Service from CBike::ProcessControl
                  CPatch::NOP(g_libGTASA + 0x4F741E, 2); 	// CAEVehicleAudioEntity::Service from CBoat::ProcessControl
                  CPatch::NOP(g_libGTASA + 0x50AB4A, 2); 	// CAEVehicleAudioEntity::Service from CTrain::ProcessControl
                  CPatch::NOP(g_libGTASA + 0x3D6FDC, 2); 	// CHud::DrawVehicleName from CHud::Draw
	CPatch::NOP(g_libGTASA + 0x39A1D8, 2);  // CFont::PrintString from DisplayFPS
	CPatch::NOP(g_libGTASA + 0x2E1EDC, 2); 	// CUpsideDownCarCheck::UpdateTimers from CTheScripts::Process
	CPatch::NOP(g_libGTASA + 0x519116, 2);	// byte_9BAA70 from CVehicle::DoHeadLightBeam
  	CPatch::NOP(g_libGTASA + 0x51911C, 4);  // byte_9BAA71 from CVehicle::DoHeadLightBeam
  	CPatch::NOP(g_libGTASA + 0x519198, 4);  // byte_9BAA94 from CVehicle::DoHeadLightBeam
  	CPatch::NOP(g_libGTASA + 0x5191D0, 4);  // unk_9BAB00 from CVehicle::DoHeadLightBeam
  	CPatch::NOP(g_libGTASA + 0x5191DC, 2);	// unk_9BAB02 from CVehicle::DoHeadLightBeam
	CPatch::NOP(g_libGTASA + 0x45A4C8, 11);	// CWorld::FindPlayerSlotWithPedPointer from CPlayerPed::ProcessControl
  	CPatch::NOP(g_libGTASA + 0x45A4E0, 3);	// CPlayerPed::ProcessWeaponSwitch from CPlayerPed::ProcessControl
  	CPatch::NOP(g_libGTASA + 0x39B2C0, 2);	// CRealTimeShadowManager::Update from Idle
	CPatch::NOP(g_libGTASA + 0x2C2C22, 4); 	// CEntryExit::GenerateAmbientPeds from CEntryExit::TransitionFinished
	CPatch::NOP(g_libGTASA + 0x4612A0, 9); 	// CPedIntelligence::SetPedDecisionMakerType from CPopulation::AddPedAtAttractor	
	CPatch::NOP(g_libGTASA + 0x4874E0, 5);  // CPedIntelligence::AddTaskPrimaryMaybeInGroup from CTaskComplexEnterCar::CreateNextSubTask
	CPatch::NOP(g_libGTASA + 0x4045D2, 1);	// CStreaming::ms_memoryAvailable = (int)v24
	CPatch::NOP(g_libGTASA + 0x3A019C, 2);	// CRealTimeShadowManager::ReturnRealTimeShadow from CPhysical::~CPhysical
	CPatch::NOP(g_libGTASA + 0x458D68, 2);	// CFire::Extinguish from CPlayerPed::SetInitialState
	CPatch::NOP(g_libGTASA + 0x3688EC, 2);	// CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire from CAudioEngine::Service
	CPatch::NOP(g_libGTASA + 0x39844E, 2);  // CCarFXRenderer::Shutdown from CGame::Shutdown
                  CPatch::NOP(g_libGTASA + 0x39845E, 2);	// CCarFXRenderer::Shutdown from CGame::Shutdown
                  CPatch::NOP(g_libGTASA + 0x39840A, 2);	// CStreaming::Shutdown from CGame::Shutdown
	CPatch::NOP(g_libGTASA + 0x398768, 2); 	// CHeli::UpdateHelis from CGame::Process
	CPatch::NOP(g_libGTASA + 0x3987DC, 2); 	// CHeli::UpdateHelis from CGame::Process
//	CPatch::NOP(g_libGTASA + 0x398A48, 2);	// CGarages::Update from CGame::Process
	CPatch::NOP(g_libGTASA + 0x3987BA, 2);	// CCheat::DoCheats from CGame::Process
	CPatch::NOP(g_libGTASA + 0x39872A, 2);	// CCover::Update from CGame::Process
	CPatch::NOP(g_libGTASA + 0x3AC8B2, 2); 	// CMessages::AddBigMessage from CPlayerInfo::KillPlayer
	CPatch::NOP(g_libGTASA + 0x4F75B4, 4);  // CBoat::ProcessControl
	CPatch::NOP(g_libGTASA + 0x454A88, 2);  // CCamera::ClearPlayerWeaponMode from CPlayerPed::ClearWeaponTarget
	CPatch::NOP(g_libGTASA + 0x2FEE76, 2);	// CGarages::RespraysAreFree = true in CRunningScript::ProcessCommands800To899
	CPatch::NOP(g_libGTASA + 0x50FF64, 2);	// skip playerGifts from CVehicle::SetDriver

	CPatch::Write<uint8_t>(g_libGTASA + 0x385D6A, 0); 		// disable cinematic camera for trains
	CPatch::Write<uint8_t>(g_libGTASA + 0x60FDE4, 0); 		// Patch for creating Freight (537 train) without a trailer
	CPatch::Write<uint8_t>(g_libGTASA + 0x60FDE5, 0); 		// Patch for creating Freight (537 train) without a trailer
   //             CPatch::Write<uint8_t>(g_libGTASA + 0x2F7B68, 0xBE); 	// camera_on_actor path ЗАКОММЕНТИЛ (ФИКС СПЕКТАТОРА)
	CPatch::Write<uint8_t>(g_libGTASA + 0x4EE200, 0x9B); 	// CBike::ProcessAI
	CPatch::Write<float>(g_libGTASA + 0x3BAC68, 176.0f);	// MaxHealth
	CPatch::Write<float>(g_libGTASA + 0x27D884, 176.0f); 	// MaxArmour

	//CPatch::WriteMemory(g_libGTASA + 0x27E072, "\x2E", 1); // CWidgetPlayerInfo::Draw - Money: 0xBC ~ 0x2E
	//CPatch::WriteMemory(g_libGTASA + 0x27E09C, "\x2E", 1); // CWidgetPlayerInfo::Draw - Money: 0xBC ~ 0x2E
	
	CPatch::WriteMemory(g_libGTASA + 0x3981EC, "\x06\x20", 2);							// CdStreamInit(6);
	CPatch::WriteMemory(g_libGTASA + 0x1859FC, "\x01\x22", 2); 							// _rwOpenGLRasterCreate
	CPatch::WriteMemory(g_libGTASA + 0x2C3868, "\x00\x20\x70\x47", 4); 					// CGameLogic::IsCoopGameGoingOn
	CPatch::WriteMemory(g_libGTASA + 0x1A7EF2, "\x4F\xF4\x00\x10\x4F\xF4\x80\x00", 8); 	// RenderQueue::RenderQueue
	CPatch::WriteMemory(g_libGTASA + 0x1A7F32, "\x4F\xF4\x00\x10\x4F\xF4\x80\x00", 8); 	// RenderQueue::RenderQueue
	CPatch::WriteMemory(g_libGTASA + 0x1A7ECE, "\x4F\xF0\x01\x00\x00\x46", 6);			// RenderQueue::RenderQueue
	CPatch::WriteMemory(g_libGTASA + 0x45FC20, "\x4F\xF0\x00\x00\xF7\x46", 6);			// CPopulation::AddToPopulation
	CPatch::WriteMemory(g_libGTASA + 0x293218, "\x01\x20\x70\x47", 4); 					// CStreaming::RemoveLoadedVehicle
	CPatch::WriteMemory(g_libGTASA + 0x1BDD4A, "\x10\x46\xA2\xF1\x04\x0B", 6); 			// TextureDatabaseRuntime::LoadFullTexture
	CPatch::WriteMemory(g_libGTASA + 0x3E1A2C, "\x67\xE0", 2);							// Interior_c::Init
                  CPatch::WriteMemory(g_libGTASA + 0x27D8D0, "\x4F\xF0\x00\x08", 4);					// CWidgetPlayerInfo::DrawWanted
	CPatch::WriteMemory(g_libGTASA + 0x458ED1, "\xE0", 1);								// CPlayerPed::CPlayerPed
	CPatch::WriteMemory(g_libGTASA + 0x3C5B58, "\x02\x21", 2); 							// CWorld::Process
	
	//CPatch::JMPCode(g_libGTASA + 0x45F74C, g_libGTASA + 0x45F76C); // CPopulation::AddPedInCar - skip some wtf stuff
	CPatch::NOP(g_libGTASA + 0x45F74C, 16); // CPopulation::AddPedInCar - skip some wtf stuff
	
	// CTxdStore::Initialise
	CPatch::WriteMemory(g_libGTASA + 0x55BA9A, "\x4F\xF4\xB8\x50\xC0\xF2\x11\x00", 8);
	CPatch::WriteMemory(g_libGTASA + 0x55BA9E, "\xC0\xF2\x11\x00", 4);
	CPatch::WriteMemory(g_libGTASA + 0x55BAA8, "\x44\xF6\x20\x60", 4);
	CPatch::WriteMemory(g_libGTASA + 0x55BAB0, "\x44\xF6\x20\x62", 4);

	// CPlaceable::InitMatrixArray
	CPatch::WriteMemory(g_libGTASA + 0x3ABB0A, "\x4F\xF4\x7A\x61", 4);

	CPatch::WriteMemory(g_libGTASA + 0x3AF48E, "\x4C\xF6\xD0\x70\xC0\xF2\x17\x00", 8); // MOV R0, #0x17CFD0 | size=0x30 (old: 0x76F20)
 	CPatch::WriteMemory(g_libGTASA + 0x3AF49C, "\x47\xF6\xFF\x60", 4); // MOVW R0, #0x7EFF
 	CPatch::WriteMemory(g_libGTASA + 0x3AF4A4, "\x47\xF6\xFF\x62", 4); // MOVW R2, #0x7EFF

 	// CVehicleModelInfo::SetupCommonData ~ Increase matrix
                  CPatch::WriteMemory(g_libGTASA + 0x405338, "\x4F\xF6\xC0\x50", 4);	// MOV  R0, #0xFDC0
                  CPatch::WriteMemory(g_libGTASA + 0x405342, "\x50\x20", 2);			// MOVS R0, #0x50
                  CPatch::WriteMemory(g_libGTASA + 0x405348, "\x50\x22", 2);			// MOVS R2, #0x50
                  CPatch::WriteMemory(g_libGTASA + 0x405374, "\x50\x2B", 2);			// CMP  R3, #0x50

	// reallocate CPools::ms_pEntryInfoNodePool
	CPatch::WriteMemory(g_libGTASA+0x3AF27A, "\x4f\xf4\x20\x40", 4); // MOV.W	R0, #0xA000 | size = 0x14

	CPatch::WriteMemory(g_libGTASA+0x3AF284, "\x4f\xf4\x00\x60", 4); // MOV.W R0, #0x800
	CPatch::WriteMemory(g_libGTASA+0x3AF28C, "\x4f\xf4\x00\x62", 4); // MOV.W R2, #0x800
	CPatch::WriteMemory(g_libGTASA+0x3AF2BA, "\xb3\xf5\x00\x6f", 4); // CMP.W R3, #0x800

	CPatch::WriteMemory(g_libGTASA+0x3AF284, "\x4f\xf4\x90\x50", 4); // MOV.W R0, #0x1200
	CPatch::WriteMemory(g_libGTASA+0x3AF28C, "\x4f\xf4\x90\x52", 4); // MOV.W R2, #0x1200
	CPatch::WriteMemory(g_libGTASA+0x3AF2BA, "\xb3\xf5\x90\x5f", 4); // CMP.W R3, #0x1200

	// reallocate CPools::ms_pPtrNodeDoubleLinkPool
	CPatch::WriteMemory(g_libGTASA+0x3AF21C, "\x4F\xF4\x00\x30", 4); // MOV.W R0, #0x20000
	
	// Stop it trying to load tracks2.dat
	CPatch::NOP(g_libGTASA + 0x508F36, 2);

	// Stop it trying to load tracks4.dat
	CPatch::NOP(g_libGTASA + 0x508F54, 2);
	
	// new patches
	
	// CPed pool (old: 140, new: 210)
 	/* 	MOVW R0, #0x5EC8
 		MOVT R0, #6 */
 	CPatch::WriteMemory(g_libGTASA+0x3AF2D0, "\x45\xF6\xC8\x60\xC0\xF2\x06\x00", 8); // MOV  R0, #0x0065EC8 | size=0x7C4 (old: 0x43F30)
 	CPatch::WriteMemory(g_libGTASA+0x3AF2DE, "\xD2\x20", 2); // MOVS R0, #0xD2
 	CPatch::WriteMemory(g_libGTASA+0x3AF2E4, "\xD2\x22", 2); // MOVS R2, #0xD2
 	CPatch::WriteMemory(g_libGTASA+0x3AF310, "\xD2\x2B", 2); // CMP  R3, #0xD2

 	// CPedIntelligence pool (old: 140, new: 210)
	// movw r0, #0x20B0
 	// movt r0, #2
 	// nop
 	CPatch::WriteMemory(g_libGTASA+0x3AF7E6, "\x42\xF2\xB0\x00\xC0\xF2\x02\x00\x00\x46", 10); // MOVS R0, #0x00220B0 | size=0x298 (old: 0x0016B20)
 	CPatch::WriteMemory(g_libGTASA+0x3AF7F6, "\xD2\x20", 2); // MOVS R0, #0xD2
 	CPatch::WriteMemory(g_libGTASA+0x3AF7FC, "\xD2\x22", 2); // MOVS R2, #0xD2
 	CPatch::WriteMemory(g_libGTASA+0x3AF824, "\xD2\x2B", 2); // CMP  R3, 0xD2

 	// Task pool (old: 500, new: 1524 (1536))
 	CPatch::WriteMemory(g_libGTASA+0x3AF4EA, "\x4F\xF4\x40\x30", 4); // MOV.W R0, #30000 | size = 0x80 (old: 0xFA00)
 	CPatch::WriteMemory(g_libGTASA+0x3AF4F4, "\x4F\xF4\xC0\x60", 4); // MOV.W R0, #0x600
 	CPatch::WriteMemory(g_libGTASA+0x3AF4FC, "\x4F\xF4\xC0\x62", 4); // MOV.W R2, #0x600
 	CPatch::WriteMemory(g_libGTASA+0x3AF52A, "\xB3\xF5\xC0\x6F", 4); // CMP.W R3, #0x600
		
	// NOP calling CCamera::ClearPlayerWeaponMode from CPlayerPed::ClearWeaponTarget
	CPatch::NOP(g_libGTASA+0x454A88, 2);
}

int test_pointsArray[1000];
int test_pointersLibArray[1000];

void ApplyPatches_level0()
{
	// НОВЫЕ ПАТЧИ (УРЕЗАНИЕ РАЗМЕРА ИГРЫ)
	CPatch::WriteMemory(g_libGTASA + 0x573640, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x573668, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x57367C, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x573690, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x5736C8, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x5736D8, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x5736E8, (int)"dxt", 3);
	// reallocate CWorld::Players[]
	PLAYERS_REALLOC = new char[404*MAX_PLAYERS];
	CPatch::Write<char*>(g_libGTASA + 0x5D021C, PLAYERS_REALLOC);

	// 3 touch begin
	CPatch::UnFuck(g_libGTASA + 0x5D1E8C);
	memset(test_pointsArray, 0, 999 * sizeof(int));
	*(int**)(g_libGTASA + 0x005D1E8C) = &test_pointsArray[0];

	CPatch::UnFuck(g_libGTASA + 0x63D4F0);
	memset(test_pointersLibArray, 0, 999 * sizeof(int));
	*(int**)(g_libGTASA + 0x0063D4F0) = &test_pointersLibArray[0];

	CPatch::WriteMemory(g_libGTASA + 0x238232, (uintptr_t)"\x03\x20", 2);
	CPatch::WriteMemory(g_libGTASA + 0x25C522, (uintptr_t)"\x02\x2C", 2);
	// 3 touch end

    	ApplyETCpatches();

	// FIX SHADOW CRASH by KRUTOYVR

	// Shadow crash fix
	//CPatch::NOP(g_libGTASA + 0x39B2C0, 2);
	//CPatch::NOP(g_libGTASA + 0x3A019C, 2);
	CPatch::NOP(g_libGTASA + 0x39B2C4, 2);

	/*// Fix shadows crash (thx War Drum Studios)
	CPatch::UnFuck(g_libGTASA + 0x39B2C4);
	CPatch::NOP(g_libGTASA + 0x39B2C4, 2);*/
	//ExitSocialClub();

	// новые патчи 08.06.21 {
	//CPatch::WriteMemory(g_libGTASA + 0x29554A, (uintptr_t)"\x4f\xf4\x61\x60", 4); // allocate memory to 300 * sizeof(CCollisionLink)
	//CPatch::WriteMemory(g_libGTASA + 0x295556, (uintptr_t)"\x4f\xf4\x5b\x62", 4); // BUT MAKE INITIALIZED ONLY 292 DUE TO SHIT ARM ASM!! (292 * sizeof(CCollisionLink)
	// Prevent another ped from answering when collision happens (or damage)
	CPatch::NOP(g_libGTASA + 0x327730, 2);
	CPatch::NOP(g_libGTASA + 0x32C488, 2);
	CPatch::NOP(g_libGTASA + 0x32DF94, 2);
	CPatch::NOP(g_libGTASA + 0x32E05E, 2);
	CPatch::NOP(g_libGTASA + 0x32E160, 2);
	CPatch::NOP(g_libGTASA + 0x32E1F2, 2);
	CPatch::NOP(g_libGTASA + 0x32E292, 2);
	CPatch::NOP(g_libGTASA + 0x32EFD0, 2);

	// CPed::Say
	CPatch::NOP(g_libGTASA + 0x43E288, 2);

	// Disable in-game radio
	CPatch::NOP(g_libGTASA + 0x3688DA, 2);
	CPatch::NOP(g_libGTASA + 0x368DF0, 2);
	CPatch::NOP(g_libGTASA + 0x369072, 2);
	CPatch::NOP(g_libGTASA + 0x369168, 2);


}

struct _ATOMIC_MODEL
{
	uintptr_t func_tbl;
	char data[56];
};
struct _ATOMIC_MODEL *ATOMIC_MODELS;

void ApplyPatches()
{
	Log("Installing patches..");
	
	// allocate Atomic models pool
	ATOMIC_MODELS = new _ATOMIC_MODEL[120000];
	for (int i = 0; i < 120000; i++)
	{
		// CBaseModelInfo::CBaseModelInfo
		((void(*)(_ATOMIC_MODEL*))(g_libGTASA + 0x33559C + 1))(&ATOMIC_MODELS[i]);
		ATOMIC_MODELS[i].func_tbl = g_libGTASA + 0x5C6C68;
		memset(ATOMIC_MODELS[i].data, 0, sizeof(ATOMIC_MODELS->data));
	}
	
	CPatch::Write<uint32_t>(g_libGTASA + 0x8E87E4, 1);
}

void ApplyInGamePatches()
{
	Log("Installing patches (ingame)..");

	// CTheZones::ZonesVisited[100]
	memset((void*)(g_libGTASA + 0x8EA7B0), 1, 100);
	// CTheZones::ZonesRevealed
	CPatch::Write<uint32_t>(g_libGTASA + 0x8EA7A8, 1);
	// фикс артефактов при разгоне на тс 100+ км/ч
	*(float*)(g_libGTASA+0x608558) = 200.0f;
	// camera_on_actor path
	CPatch::UnFuck(g_libGTASA + 0x2F7B68);
	*(uint8_t*)(g_libGTASA + 0x2F7B6B) = 0xBE;

}
