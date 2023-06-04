#pragma once

class CAudioStream
{
public:
	// Check active
	bool m_bPlayingAudio;

public:
	CAudioStream();

	void Initialize();
	void Play(char const*, float, float, float, float, bool);
	void Stop(bool);
	void Process();

private:
	bool m_bChannelPause;
};
