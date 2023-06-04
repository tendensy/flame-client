#pragma once
#include "main.h"

#include <audio/bass/bass.h>
#include <audio/bass/bass_fx.h>
#include <audio/opus/opus.h>
#include <audio/speex/speex.h>
#include <audio/speex/speex_echo.h>
#include <audio/speex/speex_preprocess.h>

extern int (*BASS_Init)(uint32_t, uint32_t, uint32_t);
extern int (*BASS_Free)(void);
extern int (*BASS_SetConfigPtr)(uint32_t, const char*);
extern int (*BASS_SetConfig)(uint32_t, uint32_t);
extern int (*BASS_ChannelStop)(uint32_t);
extern int (*BASS_StreamCreateURL)(char*, uint32_t, uint32_t, uint32_t);
extern int (*BASS_StreamCreate) (uint32_t, uint32_t, uint32_t, STREAMPROC *, void *);
extern int (*BASS_ChannelPlay)(uint32_t, bool);
extern int (*BASS_ChannelPause)(uint32_t);
extern int *BASS_ChannelGetTags;
extern int *BASS_ChannelSetSync;
extern int *BASS_StreamGetFilePosition;
extern int (*BASS_StreamFree)(uint32_t);
extern int (*BASS_ErrorGetCode) (void);
extern int (*BASS_Set3DFactors) (float, float, float);
extern int (*BASS_Set3DPosition) (const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *);
extern int (*BASS_Apply3D) (void);
extern int (*BASS_ChannelSetFX) (uint32_t, HFX);
extern int (*BASS_ChannelRemoveFX) (uint32_t, HFX);
extern int (*BASS_FXSetParameters) (HFX, const void *);
extern int (*BASS_IsStarted) (void);
extern int (*BASS_RecordGetDeviceInfo) (uint32_t, BASS_DEVICEINFO *);
extern int (*BASS_RecordInit) (int);
extern int (*BASS_RecordGetDevice) (void);
extern int (*BASS_RecordFree) (void);
extern int (*BASS_RecordStart) (uint32_t, uint32_t, uint32_t, RECORDPROC *, void *);
extern int (*BASS_ChannelSetAttribute) (uint32_t, uint32_t, float);
extern int (*BASS_ChannelGetData) (uint32_t, void *, uint32_t);
extern int (*BASS_RecordSetInput) (int, uint32_t, float);
extern int (*BASS_StreamPutData) (uint32_t, const void *, uint32_t);
extern int (*BASS_ChannelSetPosition) (uint32_t, uint64_t, uint32_t);
extern int (*BASS_ChannelIsActive) (uint32_t);
extern int (*BASS_ChannelSlideAttribute) (uint32_t, uint32_t, float, uint32_t);
extern int (*BASS_ChannelSet3DAttributes) (uint32_t, int, float, float, int, int, float);
extern int (*BASS_ChannelSet3DPosition) (uint32_t, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *);
extern int (*BASS_SetVolume) (float);

void LoadBassLibrary();
