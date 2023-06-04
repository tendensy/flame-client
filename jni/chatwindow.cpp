#include "main.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "keyboard.h"
#include "settings.h"
#include "game/game.h"
#include "net/netgame.h"
#include "dialog.h"
#include "scoreboard.h"

auto open_time = std::chrono::steady_clock::now();

uint32_t MAX_CHAT_MESSAGES = 40; // 8

extern CGUI* pGUI;
extern CKeyBoard* pKeyBoard;
extern CSettings* pSettings;
extern CNetGame* pNetGame;
extern CAMERA_AIM* pcaInternalAim;
extern CGame* pGame;
extern CDialogWindow* pDialogWindow;
extern CScoreBoard* pScoreBoard;

void ChatWindowInputHandler(const char* str)
{
	if (!str || *str == '\0') return;
	if (!pNetGame) return;

	if (*str == '/')
		pNetGame->SendChatCommand(str);
	else
		pNetGame->SendChatMessage(str);
	return;
}


CChatWindow::CChatWindow()
{
	Log("Initializng Chat Window..");

	m_fChatPosX = pGUI->ScaleX(pSettings->Get().fChatPosX);
	m_fChatPosY = pGUI->ScaleY(pSettings->Get().fChatPosY);

	m_fChatSizeX = pGUI->ScaleX(pSettings->Get().fChatSizeX);
	m_fChatSizeY = pGUI->ScaleY(pSettings->Get().fChatSizeY);

	m_iMaxMessages = pSettings->Get().iChatMaxMessages;

	Log("Chat pos: %f, %f, size: %f, %f", m_fChatPosX, m_fChatPosY, m_fChatSizeX, m_fChatSizeY);

	m_dwTextColor = 0xFFFFFFFF;
	m_dwInfoColor = 0x00C8C8FF;
	m_dwDebugColor = 0xBEBEBEFF;

	MAX_CHAT_MESSAGES = m_iMaxMessages * 5;

	m_bIsOpened = false;
	m_FirstLine = false;
	m_NeedOpen = false;
	b_NeedYScale = false;
	m_bIsShow = true;
	m_iOffsetY = 0;

	open_time = std::chrono::steady_clock::now();
}

CChatWindow::~CChatWindow()
{
}

bool CChatWindow::CheckScrollBar(int x, int y)
{
	float size = pGUI->GetFontSize() * m_iMaxMessages;
	float scrollBarSize = m_ChatWindowEntries.size() * (MAX_CHAT_MESSAGES / m_iMaxMessages);

	if (m_bIsOpened && x >= m_fChatPosX - pGUI->ScaleX(40.0f) && x <= m_fChatPosX - pGUI->ScaleX(3.0f) &&
		y >= m_fChatPosY + pGUI->ScaleY(scrollBarSize) + m_iOffsetY && y <= m_fChatPosY + size + m_iOffsetY)
	{
		bSwipeScroll = true;
		//m_bIsOpened = true;
		return true;
	}
	return false;
}

void CChatWindow::OnExitFromInput()
{
	m_bIsOpened = false;
	bSwipeScroll = false;
	m_NeedOpen = false;

	//pGame->DisplayHUD(true);
	//pGame->DisplayWidgets(true);

	m_fChatPosX = pGUI->ScaleX(pSettings->Get().fChatPosX);
	m_fChatSizeX = pGUI->ScaleX(pSettings->Get().fChatSizeX);

	ResetHiding();
}

bool CChatWindow::OnTouchEvent(int type, bool multi, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();

	static bool bWannaOpenChat = false;

	float size = pGUI->GetFontSize() * m_iMaxMessages;
	float scrollBarSize = m_ChatWindowEntries.size() * (MAX_CHAT_MESSAGES / m_iMaxMessages);

	switch (type)
	{
	case TOUCH_PUSH:
		if (m_bIsOpened && x >= m_fChatPosX - pGUI->ScaleX(40.0f) && x <= m_fChatPosX - pGUI->ScaleX(3.0f) &&
			y >= m_fChatPosY + pGUI->ScaleY(scrollBarSize) + m_iOffsetY &&
			y <= m_fChatPosY + size + m_iOffsetY)
		{
			bSwipeScroll = true;
			return true;
		}
		if (x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
			y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
			bWannaOpenChat = true;
		break;

	case TOUCH_POP:
		if (m_bIsOpened && bSwipeScroll)
		{
			bSwipeScroll = false;
			return true;
		}
		if (bWannaOpenChat &&
			x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
			y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
		{
			if (!(pDialogWindow->m_bIsActive == true &&
			    (
			            pDialogWindow->m_byteDialogStyle == DIALOG_STYLE_LIST ||
			            pDialogWindow->m_byteDialogStyle == DIALOG_STYLE_TABLIST ||
			            pDialogWindow->m_byteDialogStyle == DIALOG_STYLE_TABLIST_HEADERS ||
			            pDialogWindow->m_byteDialogStyle == DIALOG_STYLE_MSGBOX
			    )))
			{
				m_bIsOpened = true;
				pKeyBoard->Open(&ChatWindowInputHandler);

				//pGame->DisplayHUD(false);
				//pGame->DisplayWidgets(false);

				// Двигает чат на место карты
				//m_fChatPosX = 70; 
				//m_fChatSizeX = io.DisplaySize.x - ImGui::GetFontSize() * 3;

				auto current = std::chrono::steady_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current - open_time);
				if (elapsed.count() > 5000) 
				{
					m_NeedOpen = true;
					ResetHiding();
				}
				else
				{
					m_NeedOpen = false;
					ResetHiding();
				}
				b_NeedYScale = false;
				m_FirstLine = false;
			}
		}
		bWannaOpenChat = false;
		break;

	case TOUCH_MOVE:
		if (m_bIsOpened && bSwipeScroll)
		{
			if (m_iLastPosY > y)
			{
				if (m_fChatPosY + pGUI->ScaleY(scrollBarSize) + m_iOffsetY > m_fChatPosY)
					m_iOffsetY -= 3;
			}
			if (m_iLastPosY < y)
			{
				if (m_fChatPosY + size + m_iOffsetY <= m_fChatPosY + size)
					m_iOffsetY += 3;
			}

			m_iLastPosY = y;
			return false;
		}
		break;
	}

	return true;
}


void CChatWindow::Render()
{
    if (pDialogWindow->m_bIsActive || pScoreBoard->GetState() || !m_bIsShow)
        return;
		
	if (pSettings->Get().bDebug)
	{
		ImGui::GetBackgroundDrawList()->AddRect(
			ImVec2(m_fChatPosX, m_fChatPosY),
			ImVec2(m_fChatPosX + m_fChatSizeX, m_fChatPosY + m_fChatSizeY),
			IM_COL32_BLACK
		);
	}

	float size = pGUI->GetFontSize() * m_iMaxMessages;
	ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY + size - pGUI->GetFontSize());
	float scrollBarSize = m_ChatWindowEntries.size() * (MAX_CHAT_MESSAGES / m_iMaxMessages);

	if (m_bIsOpened)
	{
		if (m_ChatWindowEntries.size() > m_iMaxMessages)
		{
			ImGui::GetOverlayDrawList()->AddRectFilled(
				ImVec2(m_fChatPosX - pGUI->ScaleX(40.0f + pSettings->Get().iFontOutline), m_fChatPosY + size + m_iOffsetY + pSettings->Get().iFontOutline), //
				ImVec2(m_fChatPosX - pGUI->ScaleX(3.0f - pSettings->Get().iFontOutline),
					m_fChatPosY + pGUI->ScaleY(scrollBarSize) + m_iOffsetY - pSettings->Get().iFontOutline), // m_iOffsetY
				0xB0000000
			);
			ImGui::GetOverlayDrawList()->AddRectFilled(
				ImVec2(m_fChatPosX - pGUI->ScaleX(40.0f), m_fChatPosY + size + m_iOffsetY), //
				ImVec2(m_fChatPosX - pGUI->ScaleX(3.0f),
					m_fChatPosY + pGUI->ScaleY(scrollBarSize) + m_iOffsetY), // m_iOffsetY
				ImColor(col_all1, col_all2, col_all3, 0xFF)
			);
		}
	}
	float scrollbarSize = (m_fChatPosY + size + m_iOffsetY) - (m_fChatPosY + pGUI->ScaleY(scrollBarSize) + m_iOffsetY);
	float scroll = size / scrollbarSize;

	int counter = -1;
	int counterEx = 0;

	for (std::list<CHAT_WINDOW_ENTRY>::iterator entry = m_ChatWindowEntries.end(); entry != m_ChatWindowEntries.begin(); entry--)
	{
		//
		counter++;
		if (-(m_iOffsetY / scroll) > counter && m_iOffsetY / scroll < 0)
		{
			continue;
		}

		counterEx++;
		if (counterEx > m_iMaxMessages)
		{
			continue;
		}

		if (counter == 0 && m_iOffsetY <= 3)
			continue;


		char buff[256];
		switch (entry->eType)
		{
		case CHAT_TYPE_CHAT:
			sprintf(buff, "%s", entry->szNick);

			if (entry->szNick[0] != 0)
			{
				if (pGUI->timestamp)
					sprintf(buff, "[%s] %s %s", entry->time, entry->szNick, entry->utf8Message);
				else sprintf(buff, "%s %s", entry->szNick, entry->utf8Message, entry);

				RenderText(buff, pos.x, pos.y, entry->dwNickColor, counterEx);
			}

			if (pGUI->timestamp && entry->szNick[0] == 0)
			{
				sprintf(buff, "[%s] %s", entry->time, entry->utf8Message);
				RenderText(buff, pos.x, pos.y, entry->dwTextColor, counterEx);
			}
			break;

		case CHAT_TYPE_INFO:
		case CHAT_TYPE_DEBUG:
			if (pGUI->timestamp)
			{
				sprintf(buff, "[%s] %s", entry->time, entry->utf8Message);
				RenderText(buff, pos.x, pos.y, entry->dwTextColor, counterEx);
			}
			else
				RenderText(entry->utf8Message, pos.x, pos.y, entry->dwTextColor, counterEx);
			break;
		}

		pos.x = m_fChatPosX;
		pos.y -= pGUI->GetFontSize();

	}
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color)
{
	const int hexCount = (int)(end - start);
	if (hexCount == 6 || hexCount == 8)
	{
		char hex[9];
		strncpy(hex, start, hexCount);
		hex[hexCount] = 0;

		unsigned int hexColor = 0;
		if (sscanf(hex, "%x", &hexColor) > 0)
		{
			color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
			color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
			color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
			color.w = 1.0f;

			if (hexCount == 8)
				color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;

			return true;
		}
	}

	return false;
}

void CChatWindow::RenderText(const char* u8Str, float posX, float posY, uint32_t dwColor, int FirstMessage)
{
	const char* textStart = u8Str;
	const char* textCur = u8Str;
	const char* textEnd = u8Str + strlen(u8Str);

	ImVec2 posCur = ImVec2(posX, posY);
	ImColor colorCur = ImColor(dwColor);
	ImVec4 col;

	auto current = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current - open_time);

	while (*textCur)
	{
		// {BBCCDD}
		if (textCur[0] == '{' && ((&textCur[7] < textEnd) && textCur[7] == '}'))
		{
			if (textCur != textStart)
			{
				// ---> Set alpha -- -- -- -- --
				if (elapsed.count() > 5000 && pSettings->Get().iChatShadow && !m_bIsOpened && !pDialogWindow->m_bIsActive)
				{
					const auto shade = elapsed.count() - 5000;
					uint32_t alpha;

					if (shade < 2048)
						alpha = (2048 - shade) / 8 << 24;
					else
						alpha = 0x00000000;

					colorCur = colorCur & 0x00FFFFFF | alpha;
				}
				else if (m_NeedOpen && m_bIsOpened && pSettings->Get().iChatShadow || m_FirstLine == true && FirstMessage == 2)
				{
					if (elapsed.count() < 1000)
					{
						const auto shade = elapsed.count() * 2;
						uint32_t alpha;

						if (shade < 999)
							alpha = (shade + 2048) / 8 << 24; // Красиво, если выставить (shade + 3448), но сделал умножение, т.к. нужна плавность, значение должно приходить к 4096
						else
							alpha = 0xFF000000;

						colorCur = colorCur & 0x00FFFFFF | alpha;
					}
					else m_FirstLine = false;
				}
				else
					colorCur = colorCur & 0x00FFFFFF | 0xFF000000;
				// ---> Set alpha -- -- -- -- --
				
				if (b_NeedYScale)
				{
					const auto shade = elapsed.count();
					float yMessage = (posY + pGUI->GetFontSize()) - (float(shade / 25) + 1.0); // (float(shade / &) - плавность, чем выше, тем плавнее движение
					if (elapsed.count() < 1000 && yMessage > posY)
					{
						posCur.y = yMessage;
					}
					else 
					{
						yMessage = posY;
						posCur.y = yMessage;
					}
				}

				pGUI->RenderText(posCur, colorCur, true, textStart, textCur);

				posCur.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			if (ProcessInlineHexColor(textCur + 1, textCur + 7, col))
				colorCur = col;

			textCur += 7;
			textStart = textCur + 1;
		}

		textCur++;
	}

	// --> Set alpha --> --> --> --> -->
	if (elapsed.count() > 5000 && pSettings->Get().iChatShadow && !m_bIsOpened && !pDialogWindow->m_bIsActive)
	{
		const auto shade = elapsed.count() - 5000;
		uint32_t alpha;

		if (shade < 2048)
			alpha = (2048 - shade) / 8 << 24;
		else
			alpha = 0x00000000;

		colorCur = colorCur & 0x00FFFFFF | alpha;
	}
	else if (m_NeedOpen && m_bIsOpened && pSettings->Get().iChatShadow || m_FirstLine == true && FirstMessage == 2)
	{
		if (elapsed.count() < 1000)
		{
			const auto shade = elapsed.count() * 2;
			uint32_t alpha;

			if (shade < 999)
				alpha = (shade + 2048) / 8 << 24; // Красиво, если выставить (shade + 3448), но сделал умножение, т.к. нужна плавность, значение должно приходить к 4096
			else
				alpha = 0xFF000000;
			
			colorCur = colorCur & 0x00FFFFFF | alpha;
		}
		else m_FirstLine = false;
	}
	else
		colorCur = colorCur & 0x00FFFFFF | 0xFF000000;
	// --> Set alpha --> --> --> --> -->
	
	if (b_NeedYScale)
	{
		const auto shade = elapsed.count();
		float yMessage = (posY + pGUI->GetFontSize()) - (float(shade / 25) + 1.0); // (float(shade / &) - плавность, чем выше, тем плавнее движение
		if (elapsed.count() < 1000 && yMessage > posY)
		{
			posCur.y = yMessage;
		}
		else
		{
			yMessage = posY;
			posCur.y = yMessage;
		}
	}

	if (textCur != textStart)
	{
		pGUI->RenderText(posCur, colorCur, true, textStart, textCur);
	}
	return;
}

void CChatWindow::AddChatMessage(char* szNick, uint32_t dwNickColor, char* szMessage)
{
	ResetHiding();

	FilterInvalidChars(szMessage);
	AddToChatWindowBuffer(CHAT_TYPE_CHAT, szMessage, szNick, m_dwTextColor, dwNickColor);
}

void CChatWindow::AddInfoMessage(const char* szFormat, ...)
{
	ResetHiding();

	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, tmp_buf, nullptr, m_dwInfoColor, 0);
}

void CChatWindow::AddDebugMessage(char* szFormat, ...)
{
	ResetHiding();

	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_DEBUG, tmp_buf, nullptr, m_dwDebugColor, 0);
}

void CChatWindow::AddClientMessage(uint32_t dwColor, char* szStr)
{
	ResetHiding();

	FilterInvalidChars(szStr);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, szStr, nullptr, dwColor, 0);
}

void CChatWindow::PushBack(CHAT_WINDOW_ENTRY& entry)
{
	if (m_ChatWindowEntries.size() >= MAX_CHAT_MESSAGES)
	{
		m_ChatWindowEntries.pop_front();
	}

	m_ChatWindowEntries.push_back(entry);
	return;
}

void CChatWindow::AddToChatWindowBuffer(eChatMessageType type, char* szString, char* szNick,
	uint32_t dwTextColor, uint32_t dwNickColor)
{

	int iBestLineLength = 0;
	CHAT_WINDOW_ENTRY entry;
	entry.eType = type;
	entry.dwNickColor = __builtin_bswap32(dwNickColor | 0x000000FF);
	entry.dwTextColor = __builtin_bswap32(dwTextColor | 0x000000FF);

	if (szNick)
	{
		strcpy(entry.szNick, szNick);
		strcat(entry.szNick, ":");
	}
	else
		entry.szNick[0] = '\0';

	// ---> TimeStamp -- -- -- -- --
	time_t rawtime;
	char buffer[80];
	char mes[1024];

	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	int h = timeinfo->tm_hour;
	int m = timeinfo->tm_min;
	int s = timeinfo->tm_sec;

	sprintf(buffer, "%02i:%02i:%02i", h, m, s);

	strcpy(entry.time, buffer);
	// ---> TimeStamp -- -- -- -- --

	if (type == CHAT_TYPE_CHAT && strlen(szString) > MAX_LINE_LENGTH)
	{
		iBestLineLength = MAX_LINE_LENGTH;
		// ������� ������ ������ � �����
		while (szString[iBestLineLength] != ' ' && iBestLineLength)
			iBestLineLength--;

		// ���� ��������� ����� ������ 12 ��������
		if ((MAX_LINE_LENGTH - iBestLineLength) > 12)
		{
			// ������� �� MAX_MESSAGE_LENGTH/2
			cp1251_to_utf8(entry.utf8Message, szString, MAX_LINE_LENGTH);
			PushBack(entry);

			// ������� ����� MAX_MESSAGE_LENGTH/2
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString + MAX_LINE_LENGTH);
			PushBack(entry);
		}
		else
		{
			// ������� �� �������
			cp1251_to_utf8(entry.utf8Message, szString, iBestLineLength);
			PushBack(entry);

			// ������� ����� �������
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString + (iBestLineLength + 1));
			PushBack(entry);
		}
	}
	else
	{
		cp1251_to_utf8(entry.utf8Message, szString, MAX_MESSAGE_LENGTH);
		PushBack(entry);
	}
	m_FirstLine = true;
	b_NeedYScale = true;
	return;
}

void CChatWindow::FilterInvalidChars(char* szString)
{
	while (*szString)
	{
		if (*szString > 0 && *szString < ' ')
			*szString = ' ';

		szString++;
	}
}


void CChatWindow::ResetHiding()
{
	open_time = std::chrono::steady_clock::now();
}

void CChatWindow::ToggleState()
{
	m_bIsShow = !m_bIsShow;
}