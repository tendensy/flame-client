#include "../main.h"
#include "../game/RW/RenderWare.h"
#include "../settings.h"
#include "../jniutil.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../util/patch.h"
#include "../util/util.h"
#include "interface.h"
#include "buttons.h"
#include "gamescreen.h"
#include "game/sprite2d.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CSettings *pSettings;
extern CGameScreen *pGameScreen;
extern CJavaWrapper *pJavaWrapper;

static int m_iContentId;

CInterface::CInterface() {
    m_iCurrentHudID = pSettings->Get().iHudStyle;
    m_iWantedLevel = 0;
    m_iContentId = 0;

    m_bRenderHud = true;
    m_bRenderMenu = false;

    // iBackground = (RwTexture*)LoadTextureFromDB("samp", "hud_bg");

    iHeart = (RwTexture*)LoadTextureFromDB("samp", "fh1_heart");
    iEat   = (RwTexture*)LoadTextureFromDB("samp", "fh1_eat");
    iShield= (RwTexture*)LoadTextureFromDB("samp", "fh1_shield");
    iWanted= (RwTexture*)LoadTextureFromDB("samp", "fh1_star");

    hud_bg = (RwTexture*)LoadTextureFromDB("samp", "lr_hud_bg");

    for (int i = 0; i < 47; i++)
    {
        iFist[i] = (RwTexture*)LoadTextureFromDB("samp", GetWeaponTextureName(i));
    }
    
//    iLogo = (RwTexture*)LoadTextureFromDB("samp", "flame_hud_1");
}

void CInterface::RenderHud() const {
    if (!m_bRenderHud) return;

    ImGuiIO& io = ImGui::GetIO();

    // --> Disable rendering default hud --> --> --> --> -->
    if (pSettings->Get().iHUD) 
    {
	*(uint8_t*)(g_libGTASA+0x7165E8) = 0;
    }
    else *(uint8_t*)(g_libGTASA+0x7165E8) = 1;

    if(!pGame->FindPlayerPed()->m_bPlayerControllable) return;

    if (pSettings && pSettings->Get().iHUD && pNetGame->GetGameState() == GAMESTATE_CONNECTED) // --> HUD #2 --> --> --> --> -->
    {   
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);

        ImGui::Begin("__hud_blackonline", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings
        );

        ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.5f);
        ImGui::GetStyle().WindowPadding = ImVec2(8, 8);
        ImGui::GetStyle().FrameBorderSize = 0.0f;

        ImDrawList* dList = ImGui::GetOverlayDrawList();

        float wPX = ImGui::GetWindowPos().x;
        float wPY = ImGui::GetWindowPos().y;

        CPlayerPed *pPlayerPed = pGame->FindPlayerPed();

        if(pPlayerPed)
        {
            //        -> pos X   value
            float health  = pGUI->ScaleX(140) + pGUI->ScaleX(pPlayerPed->GetHealth()) * 2;
            float eat     = pGUI->ScaleX(140) + pGUI->ScaleX((float)pGUI->m_Eat) * 2;
            float armour  = pGUI->ScaleX(140) + pGUI->ScaleX(pPlayerPed->GetArmour()) * 2;

            if (pPlayerPed->GetHealth() >= 100)
            {
                health  = pGUI->ScaleX(140) + pGUI->ScaleX(100) * 2;
            }

	    //dList->AddImage(iBackground->raster, ImVec2(io.DisplaySize.x - pGUI->ScaleX(400), ImVec2(io.DisplaySize.x - pGUI->ScaleX(400), ImVec2(io.DisplaySize.x - pGUI->ScaleX(400), pGUI->ScaleX(18))); // background
	    //ImGui::Image(iBackground->raster, ImVec2(/*io.DisplaySize.x - */pGUI->ScaleX(370), pGUI->ScaleY(350))); // ШИРИНА, ВЫСОТА

            unsigned int money = pGame->GetLocalMoney();
            int patrons = pPlayerPed->FindWeaponSlot(pPlayerPed->GetCurrentWeapon())->dwAmmo;
            int patronsInClip = pPlayerPed->FindWeaponSlot(pPlayerPed->GetCurrentWeapon())->dwAmmoInClip;
            int currentWeaponID = pPlayerPed->GetCurrentWeapon();
            char buffer[24] = "";

            ImVec2 pos = ImVec2(wPX + pGUI->ScaleX(345), wPY + pGUI->ScaleX(102));

            // --> HP --> --> --> --> -->
            sprintf(buffer, "%.0f", pPlayerPed->GetHealth());
            pGUI->RenderText(pos, ImColor(255, 255, 255, 255), false, buffer, nullptr, pGUI->ScaleY(18.0f));
            dList->AddImage(iHeart->raster, ImVec2(pos.x - pGUI->ScaleX(240), pos.y - pGUI->ScaleX(3)), ImVec2(pos.x - pGUI->ScaleX(219), pos.y + pGUI->ScaleX(18)));

            dList->AddRectFilled(ImVec2(wPX + pGUI->ScaleX(140), wPY + pGUI->ScaleX(115)), ImVec2(wPX + pGUI->ScaleX(340), wPY + pGUI->ScaleX(105)), ImColor(0.0f, 0.0f, 0.0f, 0.8f));
            dList->AddRectFilled(ImVec2(wPX + pGUI->ScaleX(140), wPY + pGUI->ScaleX(115)), ImVec2(wPX + health, wPY + pGUI->ScaleX(105)), ImColor(254, 0, 0, 255));
            // --> ARMOUR --> --> --> --> -->
	    pos.y += pGUI->ScaleX(30);

            sprintf(buffer, "%.0f", pPlayerPed->GetArmour());
            pGUI->RenderText(pos, ImColor(255, 255, 255, 255), false, buffer, nullptr, pGUI->ScaleY(18.0f));
            dList->AddImage(iShield->raster, ImVec2(pos.x - pGUI->ScaleX(240), pos.y - pGUI->ScaleX(3)), ImVec2(pos.x - pGUI->ScaleX(219), pos.y + pGUI->ScaleX(18)));

            dList->AddRectFilled(ImVec2(wPX + pGUI->ScaleX(140), wPY + pGUI->ScaleX(145)), ImVec2(wPX + pGUI->ScaleX(340), wPY + pGUI->ScaleX(135)), ImColor(0.0f, 0.0f, 0.0f, 0.8f));
            dList->AddRectFilled(ImVec2(wPX + pGUI->ScaleX(140), wPY + pGUI->ScaleX(145)), ImVec2(wPX + armour, wPY + pGUI->ScaleX(135)), ImColor(0, 120, 255, 255));
            
            // --> EAT --> --> --> --> -->
            pos.y += pGUI->ScaleX(30);

            sprintf(buffer, "%.0f", (float)pGUI->m_Eat);
            pGUI->RenderText(pos, ImColor(255, 255, 255, 255), false, buffer, nullptr, pGUI->ScaleY(18.0f));
            dList->AddImage(iEat->raster, ImVec2(pos.x - pGUI->ScaleX(240), pos.y - pGUI->ScaleX(3)), ImVec2(pos.x - pGUI->ScaleX(219), pos.y + pGUI->ScaleX(18)));

            dList->AddRectFilled(ImVec2(wPX + pGUI->ScaleX(140), wPY + pGUI->ScaleX(175)), ImVec2(wPX + pGUI->ScaleX(340), wPY + pGUI->ScaleX(165)), ImColor(0.0f, 0.0f, 0.0f, 0.8f));
            dList->AddRectFilled(ImVec2(wPX + pGUI->ScaleX(140), wPY + pGUI->ScaleX(175)), ImVec2(wPX + eat, wPY + pGUI->ScaleX(165)), ImColor(255, 168, 0, 255));
            
            pos = ImVec2(wPX + pGUI->ScaleX(40), wPY + pGUI->ScaleX(170));
            sprintf(buffer, "%d-%d", patronsInClip, patrons - patronsInClip);

            pGUI->RenderText(pos, ImColor(255, 255, 255, 255), false, buffer, nullptr, pGUI->ScaleY(20.0f));

            // --> TIME --> --> --> --> -->
            time_t i; struct tm* tm;
            time(&i); tm = localtime(&i);

            sprintf(buffer, "%02i:%02i", tm->tm_hour, tm->tm_min);

            pos = ImVec2(wPX + pGUI->ScaleX(240), wPY + pGUI->ScaleX(40));
            pGUI->RenderText(pos, ImColor(255, 255, 255, 255), false, buffer, nullptr, pGUI->ScaleY(40));

            // --> MONEY --> --> --> --> -->
            sprintf(buffer, "%s", formatInt(money).c_str());

            pos = ImVec2(wPX - pGUI->ScaleX(20), wPY + pGUI->ScaleX(200));
            pGUI->RenderText(pos, ImColor(255, 255, 255, 255), false, buffer, nullptr, pGUI->ScaleY(30.0f));

            pos = ImVec2(pos.x + ImGui::CalcTextSize(buffer).x * (30 / pSettings->Get().fFontSize), pos.y);
            pGUI->RenderText(pos, ImColor(113, 228, 0, 255), false, "р", nullptr, pGUI->ScaleY(30.0f));

            // --> WANTED --> --> --> --> -->
            if (m_iWantedLevel != 0)
            {
                pos = ImVec2(pos.x + ImGui::GetFontSize() * 1.2, pos.y - pGUI->ScaleX(3));

                for (int idx = 0; idx < m_iWantedLevel; idx++) {
                    float mult = 30.0f * (float)idx;
                    dList->AddImage(iWanted->raster, ImVec2(pos.x + mult, pos.y), ImVec2(pos.x + pGUI->ScaleX(30.0f) + mult, pos.y + pGUI->ScaleX(30.0f)));
                }
            }
        }

        ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - pGUI->ScaleX(400), 0));
        ImGui::End();
        ImGui::PopStyleColor(3);

        if(pPlayerPed)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0x00, 0x00, 0x00, 0x00).Value);

            ImGui::Begin("__lr_hud_fist", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings
                    );
            ImGui::SetWindowPos(ImVec2(wPX - pGUI->ScaleX(25), wPY + pGUI->ScaleX(23)));

            ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.5f);
            ImGui::GetStyle().WindowPadding = ImVec2(8, 8);
            ImGui::GetStyle().FrameBorderSize = 0.0f;

            int patronsInClip = pPlayerPed->FindWeaponSlot(pPlayerPed->GetCurrentWeapon())->dwAmmoInClip;
            int currentWeaponID = pPlayerPed->GetCurrentWeapon();

            if (ImGui::ImageButton(iFist[currentWeaponID]->raster, ImVec2(pGUI->ScaleX(170), pGUI->ScaleX(170))))
            {
                pNetGame->SendChatCommand(cryptor::create("/setplayergun_878787", 21).decrypt());
            }

            ImGui::End();
            ImGui::PopStyleColor(3);
        }
    }    
}

void CInterface::RenderMenu() {
    if (!m_bRenderMenu) return;
    SaveMenuSettings();

    pGUI->SetupDefaultStyle();

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Begin("#menu", nullptr,
                 ImGuiWindowFlags_NoTitleBar
                 | ImGuiWindowFlags_NoResize
                 | ImGuiWindowFlags_NoSavedSettings
                 | ImGuiWindowFlags_NoScrollbar
                 | ImGuiWindowFlags_NoMove
    );

    ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y),
            ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetFontSize() * 2),
            0xFF000000
    );

    ImGui::Text("Настройки клиента");
    ImGui::Separator();

    ImGui::BeginChild("#__menu", ImVec2(-1, ImGui::GetWindowSize().y - ImGui::GetFontSize() * 4.7f), false);

    ImGui::Separator();

    // --> Fist Click Selector --> --> --> --> -->
    const char* s_items[] = { "Меню клиента", "Список игроков", "Статистика", "Время", "Action Menu" };
    static const char* s_item_current = s_items[pSettings->Get().iFistClick];

    ImGui::Text("Выбор действия при нажатии на фист:");

    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("#__fist_click", s_item_current))
    {
        for (auto & s_item : s_items)
        {
            bool is_selected = (s_item_current == s_item);
            if (ImGui::Selectable(s_item, is_selected))
                s_item_current = s_item;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if(s_item_current == s_items[0]) pSettings->Get().iFistClick = 0;
    else if(s_item_current == s_items[1]) pSettings->Get().iFistClick = 1;
    else if(s_item_current == s_items[2]) pSettings->Get().iFistClick = 2;
    else if(s_item_current == s_items[3]) pSettings->Get().iFistClick = 3;
    else if(s_item_current == s_items[4]) pSettings->Get().iFistClick = 4;

    // --> Texture Memory Limit Selector --> --> --> --> -->
    const char* h_items[] = { "Выключен", "250 MB", "500 MB" };
    static const char* h_item_current = h_items[pSettings->Get().iTextureLimit];

    ImGui::Text("Увеличение памяти текстур:");

    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("#__texture_limit", h_item_current))
    {
        for (auto & h_item : h_items)
        {
            bool is_selected = (h_item_current == h_item);
            if (ImGui::Selectable(h_item, is_selected))
                h_item_current = h_item;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if(h_item_current == h_items[0]) pSettings->Get().iTextureLimit = 0;
    else if(h_item_current == h_items[1]) pSettings->Get().iTextureLimit = 1;
    else if(h_item_current == h_items[2]) pSettings->Get().iTextureLimit = 2;

	// --> KeyBoard Selector --> --> --> --> -->
    const char* k_items[] = { "Стандартная", "Стандартная с фоном", "Android клавиатура" };
    static const char* k_item_current = k_items[pSettings->Get().iKeyboard];

    ImGui::Text("Выбор клавиатуры:");

    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("#__select_keyboard", k_item_current))
    {
        for (auto & k_item : k_items)
        {
            bool is_selected = (k_item_current == k_item);
            if (ImGui::Selectable(k_item, is_selected))
                k_item_current = k_item;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if(k_item_current == k_items[0]) pSettings->Get().iKeyboard = 0;
    else if(k_item_current == k_items[1]) pSettings->Get().iKeyboard = 1;
    else if(k_item_current == k_items[2]) pSettings->Get().iKeyboard = 2;

                  // --> Checkbox`s --> --> --> --> -->
                  ImGui::Checkbox("Время в чате", &pGUI->timestamp);
                  ImGui::Checkbox("Фон 3D текста", &pGUI->bLabelBackground);
                  ImGui::Checkbox("Квадратный радар", &pSettings->Get().szRadar);
                  ImGui::Checkbox("Затухание чата", &pSettings->Get().iChatShadow);
	//TODO: ImGui::Checkbox("Сглаживание текстур", &pSettings->Get().iAntiAliasing);
                 // ImGui::Checkbox("Android клавиатура", &pSettings->Get().iAndroidKeyboard);
                  ImGui::Checkbox("Счетчик FPS", &pSettings->Get().iFPSCounter);
	ImGui::Checkbox("Прозрачная обводка радара", &pSettings->Get().iRadarAlpha);
	ImGui::Checkbox("Новый HUD", &pSettings->Get().iHUD);
	ImGui::Checkbox("Отображение координат", &pSettings->Get().iCoord);
	
	//ImGui::Text("Настройки голосового чата\n");
                  //ImGui::Checkbox("Голосовой чат", &pSettings->Get().bVoiceChatEnable);
	
	//ImGui::Text("Функции после применения которых необходимо перезайти\n");
    ImGui::Checkbox("Полный экран", &pSettings->Get().iCutout);
    if(ImGui::Checkbox("Оптимизация игры", &pSettings->Get().bnoFX))
    {
        if(pSettings->Get().bnoFX)
	{
		//CPatch::NOP(g_libGTASA + 0x39B36A, 2);
		CPatch::WriteMemory(g_libGTASA + 0x52DD38, "\x00\x20\x70\x47", 4); // return 0 CCoronas::RenderReflections
		CPatch::NOP(g_libGTASA + 0x39AD14, 1); // skip render clouds, sunrefl, raineffect 
	}
	else
	{
		//CPatch::WriteMemory(g_libGTASA + 0x39B36A, "\xFF\xF7\x5D\xFD", 4);
		CPatch::WriteMemory(g_libGTASA + 0x52DD38, "\x2D\xE9\xF0\x4F", 4); // CCoronas::RenderReflections 
		CPatch::WriteMemory(g_libGTASA + 0x39AD14, "\xB3\xB3\x03\x2B", 4); 
	}

    }
    static float fTempDist = pSettings->Get().fDrawPedDist;
    //ImGui::SliderFloat("Дистанция прорисовки игроков", &fTempDist, 10.0f, 150.0f);
    pSettings->Get().fDrawPedDist = fTempDist;
    ImGui::Separator();
    ImGui::EndChild();

    if (ImGui::Button("Закрыть", ImVec2(-1, -1)))
        m_bRenderMenu = false;
	//ImGui::SameLine(0, 5);

    ImGui::SetWindowSize(ImVec2(pGUI->ScaleY(1500), pGUI->ScaleY(700)));
    ImGui::SetWindowPos(ImVec2(((io.DisplaySize.x - ImGui::GetWindowSize().x) / 2), ((io.DisplaySize.y - ImGui::GetWindowSize().y) / 2)));

    ImGui::End();
}
