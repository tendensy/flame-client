#include <dlfcn.h>
#include "main.h"

/*
   пч о0xCOFFE
*/

#include "game/game.h"
#include "game/RW/RenderWare.h"

#include "net/netgame.h"

#include "gui/gui.h"
#include "gui/CFontRenderer.h"

#include "chatwindow.h"
#include "spawnscreen.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "settings.h"
#include "scoreboard.h"
#include "game/snapshothelper.h"
#include "deathmessage.h"
#include "util/patch.h"
#include "checkfilehash.h"
#include "GButton.h"
#include "game/audiostream.h"
#include "gui/buttons.h"
#include "gui/interface.h"
#include "game/materialtext.h"
#include "game/loadingscreen.h"
#include "gui/gamescreen.h"
#include "jniutil.h"
#include "game/radar.h"
#include "game/customplate.h"
#include "game/firstperson.h"

uintptr_t g_libGTASA = 0;
char* g_pszStorage = nullptr;

CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
CChatWindow *pChatWindow = nullptr;
CSpawnScreen *pSpawnScreen = nullptr;
CPlayerTags *pPlayerTags = nullptr;
CDialogWindow *pDialogWindow = nullptr;
CScoreBoard *pScoreBoard = nullptr;
CSnapShotHelper* pSnapShotHelper = nullptr;
CDeathMessage *pDeathMessage = nullptr;
CAudioStream *pAudioStream = nullptr;
CLoadingScreen* pLoadingScreen = nullptr;
CJavaWrapper *pJavaWrapper = nullptr;
CFirstPersonCamera *pFirstPersonCamera = nullptr;

CGUI *pGUI = nullptr;
CKeyBoard *pKeyBoard = nullptr;
CDebug *pDebug = nullptr;
CSettings *pSettings = nullptr;
CGButton *pGButton = nullptr;
CMaterialText *pMaterialText = nullptr;
CGameScreen* pGameScreen = nullptr;

#include "jniutil.h"

JavaVM* CMain::javaVM = nullptr;
char* CMain::BaseStorage = nullptr;
char* CMain::RootStorage = nullptr;

void InstallSpecialHooks();
void InitRenderWareFunctions();
void ApplyPatches_level0();
void MainLoop();

void CMain::Terminate()
{
	if(pNetGame && pNetGame->GetGameState() == GAMESTATE_CONNECTED) {
                                    Log("[ONLINE] Disconnect | Crash detect."); 
		pNetGame->GetRakClient()->Disconnect(500);
     	}
	SaveMenuSettings();
	Log("[CLIENT] GTASA Terminated :)");
};

void PrintSymbols(void* pc, void* lr)
{
	Dl_info info_pc, info_lr;
	if (dladdr(pc, &info_pc) != 0)
	{
		CrashLog("PC: %s", info_pc.dli_sname);
	}
	if (dladdr(lr, &info_lr) != 0)
	{
		CrashLog("LR: %s", info_lr.dli_sname);
	}
}

void handler(int signum, siginfo_t *info, void* contextPtr)
{
	auto* context = (ucontext_t*)contextPtr;

	CMain::Terminate();

	if(info->si_signo == SIGSEGV)
	{	
		CrashLog("[CLIENT] GTASA Terminated from crash detect");
                                    CrashLog("-----------------------------GTASA--------------------------------");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);
                                    CrashLog("-------------------SAMP---------------------");
		CrashLog("3: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
		CrashLog("4: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));
                                    CrashLog("-----------------------------CLIENT--------------------------------");
		CrashLog("BUILD DATE: %s", __DATE__);
		CrashLog("VERSION: %s", CLIENT_VER);
		CrashLog("! | GTA SA IN GAME");
		CrashLog("Patches: installed and Ingame patches: installed");
		PrintSymbols((void*)(context->uc_mcontext.arm_pc), (void*)(context->uc_mcontext.arm_lr));

		Log("[CLIENT] GTASA Terminated from crash detect");
		Log("-----------------------------GTASA--------------------------------");
		Log("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		Log("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);
                                    Log("-------------------SAMP---------------------");
		Log("3: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
		Log("4: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));
                                    Log("-----------------------------CLIENT--------------------------------");
		Log("BUILD DATE: %s", __DATE__);
		Log("VERSION: %s", CLIENT_VER);
		Log("! | GTA SA IN GAME");
		Log("Patches: installed | Ingame patches: installed");
		exit(0);
	}

	return;
}

void ProtectLibs()
{
	while (true)
	{
		/* code */
	}
	
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{	
	CMain::SetVM(vm);
	g_libGTASA = FindLibrary(cryptor::create("libGTASA.so", 12).decrypt());
	if (!g_libGTASA) std::terminate();
	srand(time(nullptr));

	CPatch::InitHookStuff();
	CMain::SetRootStorage("/data/data/com.weiktonplaceint.game/");
	CMain::SetBaseStorage("/storage/emulated/0/RealRussia/");

	LoadBassLibrary();
	InitRenderWareFunctions();
	ApplyPatches_level0();
	InstallSpecialHooks();

                  // java
	// CMain::InitSAMP(); 

	struct sigaction act{};
	act.sa_sigaction = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &act, nullptr);

	return JNI_VERSION_1_4;
}

void JNI_OnUnload(JavaVM *vm, void *reserved)
{
	
}

extern "C" {
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_initSAMP(JNIEnv *pEnv, jobject thiz)
	{
		CMain::InitSAMP();
		pJavaWrapper = new CJavaWrapper(pEnv, thiz);
		pJavaWrapper->SetUseFullScreen(pSettings->Get().iCutout);
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onInputEnd(JNIEnv *pEnv, jobject thiz, jbyteArray str)
	{
		pKeyBoard->OnNewKeyboardInput(pEnv, thiz, str);
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onEventBackPressed(JNIEnv *pEnv, jobject thiz)
	{
		pKeyBoard->Close();
		pChatWindow->OnExitFromInput();
	}
}

void CMain::InitSAMP()
{	
	if ( !*(uintptr_t *)(g_libGTASA + 0x61B298) && !((int (*)(const char *))(g_libGTASA + 0x179A20))("glAlphaFuncQCOM") )
  	{
    	        CPatch::NOP(g_libGTASA + 0x1A6164, 4);
    	        CPatch::WriteMemory(g_libGTASA + 0x1A6164, "\x70\x47", 2);
  	}
	
                  Log("Initializing SAMP..");
	g_pszStorage = CMain::GetBaseStorage();	
	pSettings = new CSettings();
	CRadarRect::SetEnabled(pSettings->Get().szRadar);
	// if(!FileCheckSum()) std::terminate();
}

void CMain::InitInMenu()
{
	pGame 			= new CGame();
	pLoadingScreen 	                  = new CLoadingScreen();
	pGame->InitInMenu();

	pGUI 			= new CGUI();
	pGameScreen		= new CGameScreen();

	pGButton 		                  = new CGButton();
	pKeyBoard 		= new CKeyBoard();
                  // to do release
	pFirstPersonCamera	= new CFirstPersonCamera();
	pChatWindow 	                  = new CChatWindow();
	pSpawnScreen 	                  = new CSpawnScreen();
	pPlayerTags 	                  = new CPlayerTags();
	pDialogWindow 	                  = new CDialogWindow();
	pScoreBoard 	                  = new CScoreBoard();
	pSnapShotHelper                       = new CSnapShotHelper();
	pDeathMessage 	                  = new CDeathMessage();
	pAudioStream 	                  = new CAudioStream();
	pMaterialText                               = new CMaterialText();
	
	CFontRenderer::Initialise();
	CCustomPlateManager::Initialise();

	pDebug			= new CDebug();
}

void CMain::InitInGame()
{
	static bool bGameInited = false;
	static bool bNetworkInited = false;

	if(!bGameInited)
	{
		pGame->InitInGame();
		pGame->SetMaxStats();

		pAudioStream->Initialize();

		if(!pSettings->Get().bOnline)
		{
			pGame->SetWorldTime(12, 0);

			CCamera* pCamera = pGame->GetCamera();
			pCamera->Restore();
			pCamera->SetBehindPlayer();
		}

		pNetGame = new CNetGame(cryptor::create("188.165.59.228", 14).decrypt(), atoi(cryptor::create("1181", 4).decrypt()), pSettings->Get().szNickName, pSettings->Get().szPassword);
		bGameInited = true;
		return;
	}
}

void MainLoop()
{
	CMain::InitInGame();
	
	if(pDebug) pDebug->Process();
	if(pNetGame) pNetGame->Process();
	if(pAudioStream) pAudioStream->Process();
	CCustomPlateManager::Process();
}