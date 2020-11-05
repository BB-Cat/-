#pragma once
//#include "Prerequisites.h"


//#include <xaudio2.h>
//typedef std::shared_ptr<IXAudio2SourceVoice> SourceVoicePtr;

//class Audio
//{
//public:
//	Audio();
//	~Audio();
//
//	SourceVoicePtr createSourceVoicePtr()
//
//private:
//	IXAudio2* m_xaudio = nullptr;
//	IXAudio2MasteringVoice* m_masterVoice = nullptr;
//};

//#include "./DirectXTK-master/Inc/Audio.h"
#include "../Include/DirectXTK-master/Inc/Audio.h"

#define MUSIC_FILE_MAX (10)

#define WAVE_FILE_MAX (30)
#define WAVE_SOUND_MAX (30)

class AudioSystem
{
private:

    static AudioSystem* a;
    AudioSystem();

public:

    static AudioSystem* get();
    ~AudioSystem();

public:

    bool reset();

    void musicLoad(int, const wchar_t*, float volume = (0.5f));
    void musicUnload(int);
    void musicPlay(int, bool isLoop = (false));
    void musicStop(int);
    void musicPause(int);
    void musicResume(int);
    void musicSetVolume(int, float);

    DirectX::SoundState musicGetState(int);

    bool musicIsLooped(int);
    void musicSetPan(int, float);
    void musicSetPitch(int, float);

    const WAVEFORMATEX* musicGetFormat(int);

    bool musicIsInUse(int);
    void soundLoad(const wchar_t*, float volume = (0.5f));
    void soundPlay(int);
    void soundSetVolume(int, float);

    bool isAudioDevicePresent() { return audioEngine->IsAudioDevicePresent(); }
    bool isCriticalError() { return audioEngine->IsCriticalError(); }

private:
    std::unique_ptr<DirectX::AudioEngine>			audioEngine;
    std::unique_ptr<DirectX::SoundEffect>			music[MUSIC_FILE_MAX];
    std::unique_ptr<DirectX::SoundEffectInstance>	musicInst[MUSIC_FILE_MAX];
    float											musicVolume[MUSIC_FILE_MAX];

    std::unique_ptr<DirectX::WaveBank>				waveBank;
    std::unique_ptr<DirectX::SoundEffectInstance>	soundInst[WAVE_FILE_MAX][WAVE_SOUND_MAX];
    float											soundVolume[WAVE_FILE_MAX];
};
