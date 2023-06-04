#include <pthread.h>
#include <dlfcn.h>

#include "../main.h"
#include "../game/game.h"
#include "../game/audiostream.h"

extern CGame* pGame;

char g_szAudioStreamUrl[256];
float g_fAudioStreamX;
float g_fAudioStreamY;
float g_fAudioStreamZ;
float g_fAudioStreamRadius;
bool g_audioStreamUsePos;
bool g_bAudioStreamStop;
pthread_t g_bAudioStreamThreadWorked;
uintptr_t bassStream;

CAudioStream::CAudioStream()
{

}
void CAudioStream::Initialize()
{
	bassStream = 0;
	BASS_SetConfigPtr(16, "SA-MP/0.3");
	BASS_Free();
	if (BASS_Init(-1, 44100, 0))
	{
		BASS_SetConfigPtr(16, "SA-MP/0.3");
		BASS_SetConfig(21, 1);
		BASS_SetConfig(11, 10000);
	}
}

void CAudioStream::Process()
{
	MATRIX4X4 mat;
	pGame->FindPlayerPed()->GetMatrix(&mat);

	VECTOR vecPlayerPos = { mat.pos.X, mat.pos.Y, mat.pos.Z };
	VECTOR vecAudioPos = { g_fAudioStreamX, g_fAudioStreamY, g_fAudioStreamZ };

    if (m_bPlayingAudio && m_bChannelPause && bassStream)
    {
        if (pGame->IsMenuActive())
        {
            BASS_ChannelPause(bassStream);
            m_bChannelPause = true;
        }
        else
        {
            if (g_audioStreamUsePos)
            {
                if (GetDistanceFromVectorToVector(&vecPlayerPos, &vecAudioPos) <= g_fAudioStreamRadius)
                {
                    BASS_ChannelPlay(bassStream, false);
                    m_bChannelPause = false;
                }
                else
                {
                    BASS_ChannelPause(bassStream);
                    m_bChannelPause = true;
                }
            }
            else
            {
                BASS_ChannelPlay(bassStream, false);
                m_bChannelPause = false;
            }
        }
    }
}

void* audioStreamThread(void* p)
{
	g_bAudioStreamThreadWorked = 1;
	if (bassStream)
	{
		BASS_ChannelStop(bassStream);
		bassStream = 0;
	}
	bassStream = BASS_StreamCreateURL(g_szAudioStreamUrl, 0, 9699328, 0);
	BASS_ChannelPlay(bassStream, false);
	while (!g_bAudioStreamStop) usleep(2000);
	BASS_ChannelStop(bassStream);
	bassStream = 0;
	g_bAudioStreamThreadWorked = 0;
	pthread_exit(nullptr);
}

void CAudioStream::Play(char const* szURL, float X, float Y, float Z, float Radius, bool bUsePos)
{
    if (g_bAudioStreamThreadWorked)
	{
		g_bAudioStreamStop = true;

		do
			usleep(2000);
		while (g_bAudioStreamThreadWorked);

		BASS_StreamFree(bassStream);
		bassStream = 0;
	}
	if (bassStream)
	{
		BASS_StreamFree(bassStream);
		bassStream = 0;
	}
	memset(g_szAudioStreamUrl, 0, 256);
	strncpy(g_szAudioStreamUrl, szURL, 256);

	g_fAudioStreamX = X;
	g_fAudioStreamY = Y;
	g_fAudioStreamZ = Z;

	g_fAudioStreamRadius = Radius;
	g_audioStreamUsePos = bUsePos;
	g_bAudioStreamStop = false;

	m_bPlayingAudio = true;
	m_bChannelPause = false;

	pthread_create(&g_bAudioStreamThreadWorked, nullptr, audioStreamThread, nullptr);
}

void CAudioStream::Stop(bool bStop)
{
	if (g_bAudioStreamThreadWorked)
	{
		g_bAudioStreamStop = true;
		if (bStop)
		{
			do
				usleep(200);
			while (g_bAudioStreamThreadWorked);
		}

		BASS_StreamFree(bassStream);
		bassStream = 0;

		m_bPlayingAudio = false;
		m_bChannelPause = false;
	}
}
