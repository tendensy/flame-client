#include "../main.h"
#include "game.h"
#include "RW/RenderWare.h"
#include "materialtext.h"
#include "../util/patch.h"

extern CGame *pGame;
extern CGUI *pGUI;

/* imgui_impl_renderware.h */
bool ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_CreateDeviceObjects();

CMaterialText::CMaterialText()
{
    m_camera = 0;
    m_zBuffer = 0;
    SetUpScene();
}

void CMaterialText::SetUpScene()
{
    m_camera = RwCameraCreate();
    m_zBuffer = RwFrameCreate();

    RwObjectHasFrameSetFrame(m_camera, m_zBuffer);
    RwCameraSetFarClipPlane(m_camera, 300.0f);
    RwCameraSetNearClipPlane(m_camera, 0.01f);

    RwV2d view = { 0.5f, 0.5f };
    RwCameraSetViewWindow(m_camera, view);
    RwCameraSetProjection(m_camera, rwPERSPECTIVE);
    RpWorld* pRwWorld = util::game::GetWorldScene();
    if (pRwWorld) {
        RpWorldAddCamera(pRwWorld, m_camera);
    }
}

static uint16_t sizes[14][2] = {
            { 32, 32 } , { 64, 32 }, { 64, 64 }, { 128, 32 }, { 128, 64 }, { 128,128 }, { 256, 32 },
            { 256, 64 } , { 256, 128 } , { 256, 256 } , { 512, 64 } , { 512,128 } , { 512,256 } , { 512,512 }
    };

RwTexture* CMaterialText::Generate(uint8_t matSize, const char* fontname, float fontSize, uint8_t bold, uint32_t fontcol, uint32_t backcol, uint8_t align, const char* szText)
{

    matSize = (matSize / 10) - 1;

    RwRaster* raster = RwRasterCreate(sizes[matSize][0] * 2, sizes[matSize][1] * 2, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
    RwTexture* bufferTexture = RwTextureCreate(raster);
    if(!raster || !bufferTexture) return 0;

    // set camera frame buffer
    m_camera->frameBuffer = raster;

    util::game::SetRenderWareCamera(m_camera);

    RwCameraClear(m_camera, (RwRGBA*)&backcol, 3);
    RwCameraBeginUpdate(m_camera);

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODENASHADEMODE);
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
    RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

    util::game::DefinedState();

    int bgB = (backcol) & 0xFF;
    int bgG = (backcol >> 8) & 0xFF;
    int bgR = (backcol >> 16) & 0xFF;
    int bgA = (backcol >> 24) & 0xFF;

    int fontB = (fontcol) & 0xFF;
    int fontG = (fontcol >> 8) & 0xFF;
    int fontR = (fontcol >> 16) & 0xFF;
    int fontA = (fontcol >> 24) & 0xFF;

    SetUpMaterial(matSize, fontname, fontSize * 0.8f, bold, ImColor(fontR, fontG, fontB, fontA), ImColor(bgR, bgG, bgB, bgA), align, szText);

    RwCameraEndUpdate(m_camera);
    return bufferTexture;
}

void CMaterialText::SetUpMaterial(uint8_t matSize, const char* fontname, float fontSize, uint8_t bold, ImColor fontcol, ImColor backcol, uint8_t align, const char* szText)
{
    ImGui::NewFrame();

    char utf8[2048];
    cp1251_to_utf8(utf8, szText, 2048);

    // text align
    ImVec2 vecPos;

    std::string strText = utf8;
    std::stringstream ssLine(strText);
    std::string tmpLine;
    int newLineCount = 0;

    Log("TEXT: %s", szText);

    switch(align)
    {
        case OBJECT_MATERIAL_TEXT_ALIGN_LEFT:
            while(std::getline(ssLine, tmpLine, '\n'))
            {
                if(tmpLine[0] != 0)
                {
                    vecPos.x = 0;
                    vecPos.y = (sizes[matSize][1] - fontSize) / 2;
                }
            }
            break;
        case OBJECT_MATERIAL_TEXT_ALIGN_CENTER:
            while(std::getline(ssLine, tmpLine, '\n'))
            {
                if(tmpLine[0] != 0)
                {
                    vecPos.x = (sizes[matSize][0] / 2) - (UIUtils::CalcTextSizeWithoutTags((char *)tmpLine.c_str(), fontSize).x / 2);
                    vecPos.y = (sizes[matSize][1] / 2) - (UIUtils::CalcTextSizeWithoutTags((char *)tmpLine.c_str(), fontSize).y / 2);
                }
            }
            break;
        case OBJECT_MATERIAL_TEXT_ALIGN_RIGHT:
            while(std::getline(ssLine, tmpLine, '\n'))
            {
                if(tmpLine[0] != 0)
                {
                    vecPos.x = (sizes[matSize][0] - UIUtils::CalcTextSizeWithoutTags((char *)tmpLine.c_str(), fontSize).x);
                    vecPos.y = (sizes[matSize][1] - fontSize);
                }
            }
            break;
    }

    if(backcol)
    {
        ImVec2 backpos = ImVec2(vecPos.x-pGUI->GetFontSize(), vecPos.y-pGUI->GetFontSize());
        ImVec2 backsize = ImVec2(fontSize+pGUI->GetFontSize(), fontSize+pGUI->GetFontSize());
        ImGui::GetBackgroundDrawList()->AddRectFilled(backpos, backsize, backcol);
    }
    vecPos.x *= 2;
    vecPos.y *= 2;
    UIUtils::drawText(vecPos, fontcol, utf8, fontSize * 2);

    ImGui::Render();
    ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());
}