#include "../main.h"
#include "game.h"
#include "firstperson.h"
#include "..//chatwindow.h"
#include "..//keyboard.h"
#include "..//scoreboard.h"
#include "..//dialog.h"

#include "..//net/netgame.h"

extern CNetGame* pNetGame;
extern CGame *pGame;
extern CScoreBoard *pScoreBoard;
extern CKeyBoard *pKeyBoard;
extern CDialogWindow *pDialogWindow;


bool CFirstPersonCamera::m_bEnabled = false;

CFirstPersonCamera::CFirstPersonCamera()
{
	p_CamToggleTexture = (RwTexture*)LoadTextureFromDB("mobile","cam-toggle");
}

CFirstPersonCamera::~CFirstPersonCamera()
{

}

extern CChatWindow* pChatWindow;
MATRIX4X4* RwMatrixMultiplyByVector(VECTOR* out, MATRIX4X4* a2, VECTOR* in);
void CFirstPersonCamera::ProcessCameraOnFoot(uintptr_t pCam, CPlayerPed* pPed)
{
	if (!m_bEnabled || *(uint8_t*)(g_libGTASA + 0x8B147E) || *(uint8_t*)(g_libGTASA + 0x8B147F))
	{
		return;
	}

	VECTOR* pVec = (VECTOR*)(pCam + 372);

	VECTOR vecOffset;
	vecOffset.X = 0.35f;
	vecOffset.Y = 0.1f;
	vecOffset.Z = 0.1f;
	/*if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIRSTPERSON).X != -1)
	{
		vecOffset.X += (float)(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIRSTPERSON).X - 200) / 100.0f;
	}
	if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIRSTPERSON).Y != -1)
	{
		vecOffset.Y += (float)(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIRSTPERSON).Y - 200) / 100.0f;
	}
	if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_FIRSTPERSON).Y != -1)
	{
		vecOffset.Z += (float)(CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_FIRSTPERSON).Y - 200) / 100.0f;
	}*/

	VECTOR vecOut;
	RwMatrixMultiplyByVector(&vecOut, &(pPed->m_HeadBoneMatrix), &vecOffset);

	if (vecOut.X != vecOut.X || vecOut.Y != vecOut.Y || vecOut.Z != vecOut.Z)
	{
		pPed->GetBonePosition(4, &vecOut);
	}
	if (vecOut.X != vecOut.X || vecOut.Y != vecOut.Y || vecOut.Z != vecOut.Z)
	{
		return;
	}

	pVec->X = vecOut.X;
	pVec->Y = vecOut.Y;
	pVec->Z = vecOut.Z;

	((RwCamera*(*)(RwCamera*, float))(g_libGTASA + 0x001AD6F4 + 1))(*(RwCamera**)(g_libGTASA + 0x95B064), 0.2f);
}

VECTOR vecAtSaved;
VECTOR vecUpSaved;
bool bsaved = false;

void CFirstPersonCamera::ProcessCameraInVeh(uintptr_t pCam, CPlayerPed* pPed, CVehicle* pVeh)
{
	if (!m_bEnabled || !pPed->GetGtaVehicle())
	{
		return;
	}

	VECTOR* pVec = (VECTOR*)(pCam + 372);

	VECTOR vecOffset;
	vecOffset.X = 0.0f;
	vecOffset.Y = 0.0f;
	vecOffset.Z = 0.6f;

	uint16_t modelIndex = pPed->GetGtaVehicle()->entity.nModelIndex;

//#ifdef GAME_EDITION_CR
	if (modelIndex >= 15072 && modelIndex <= 15080)
	{
		vecOffset.X = 0.05f;
		vecOffset.Y = 0.3f;
		vecOffset.Z = 0.45f;
		((RwCamera * (*)(RwCamera*, float))(g_libGTASA + 0x001AD6F4 + 1))(*(RwCamera * *)(g_libGTASA + 0x95B064), 0.3f);
	}
	else
	{
		((RwCamera * (*)(RwCamera*, float))(g_libGTASA + 0x001AD6F4 + 1))(*(RwCamera * *)(g_libGTASA + 0x95B064), 0.01f);
	}
/*#else
	if (modelIndex == 581 || modelIndex == 509 || modelIndex == 481 || modelIndex == 462 || modelIndex == 521 || modelIndex == 463 || modelIndex == 510 ||
		modelIndex == 522 || modelIndex == 461 || modelIndex == 468 || modelIndex == 448 || modelIndex == 586)
	{
		vecOffset.X = 0.05f;
		vecOffset.Y = 0.3f;
		vecOffset.Z = 0.45f;
		((RwCamera * (*)(RwCamera*, float))(g_libGTASA + 0x001AD6F4 + 1))(*(RwCamera * *)(g_libGTASA + 0x95B064), 0.3f);
	}
	else
	{
		((RwCamera * (*)(RwCamera*, float))(g_libGTASA + 0x001AD6F4 + 1))(*(RwCamera * *)(g_libGTASA + 0x95B064), 0.01f);
	}
#endif
	if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIRSTPERSON_VEH).X != -1)
	{
		vecOffset.X += (float)(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIRSTPERSON_VEH).X - 200) / 100.0f;
	}
	if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIRSTPERSON_VEH).Y != -1)
	{
		vecOffset.Y += (float)(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIRSTPERSON_VEH).Y - 200) / 100.0f;
	}
	if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_FIRSTPERSON_VEH).Y != -1)
	{
		vecOffset.Z += (float)(CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_FIRSTPERSON_VEH).Y - 200) / 100.0f;
	}*/

	VECTOR vecOut;
	MATRIX4X4 mat;

	memcpy(&mat, pPed->m_pPed->entity.mat, sizeof(MATRIX4X4));

	RwMatrixMultiplyByVector(&vecOut, &mat, &vecOffset);

	if (vecOut.X != vecOut.X || vecOut.Y != vecOut.Y || vecOut.Z != vecOut.Z)
	{
		pPed->GetBonePosition(4, &vecOut);
	}
	if (vecOut.X != vecOut.X || vecOut.Y != vecOut.Y || vecOut.Z != vecOut.Z)
	{
		return;
	}

	pVec->X = vecOut.X;
	pVec->Y = vecOut.Y;
	pVec->Z = vecOut.Z;

	//pPed->SetArmedWeapon(0);

	if (!pVeh)
	{
		if (!pPed->IsAPassenger())
		{
			*(uint16_t*)(pCam + 14) = 16;
		}
		return;
	}

	if (!pPed->IsAPassenger())
	{
		VECTOR vecSpeed;
		pVeh->GetMoveSpeedVector(&vecSpeed);
		float speed = sqrt((vecSpeed.X * vecSpeed.X) + (vecSpeed.Y * vecSpeed.Y) + (vecSpeed.Z * vecSpeed.Z)) * 2.0f * 100.0f;
	}
}

void CFirstPersonCamera::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
}

void CFirstPersonCamera::Render()
{
    CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
    
    if(pNetGame->GetGameState() != GAMESTATE_CONNECTED) return;
    if(pDialogWindow->m_bIsActive || pScoreBoard->GetState() || pKeyBoard->IsOpen()) return;
    
    ImGuiIO &io = ImGui::GetIO();
    
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
	
    ImGuiStyle style;
    style.FrameBorderSize = ImGui::GetStyle().FrameBorderSize;
    ImGui::GetStyle().FrameBorderSize = 0.0f;
    
    ImGui::Begin("Cam-Toggle", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);
    
    ImVec2 vecButSize_c = ImVec2(ImGui::GetFontSize() * 3+5.0f, ImGui::GetFontSize() * 3);
    
    if(ImGui::ImageButton((ImTextureID)p_CamToggleTexture->raster, vecButSize_c))
    {
	if(!m_bEnabled) SetEnabled(true);
	else SetEnabled(false);
    }
    
    ImGui::SetWindowSize(ImVec2(-1, -1));

    ImVec2 dSize    = io.DisplaySize;
    ImVec2 winPos   = ImVec2(dSize.x - (dSize.x / 9), dSize.y / 5);

    ImGui::SetWindowPos(winPos);
    ImGui::End();
    
    ImGui::PopStyleColor(3);
    ImGui::GetStyle().FrameBorderSize = style.FrameBorderSize;
}

void CFirstPersonCamera::Toggle()
{
	m_bEnabled ^= 1;
}

bool CFirstPersonCamera::IsEnabled()
{
	return m_bEnabled;
}
