#include "MyAudio.h"
#include <exception>

//Audio::Audio()
//{
//	HRESULT res;
//	if (FAILED(res = XAudio2Create(&m_xaudio, 0, XAUDIO2_DEFAULT_PROCESSOR)))
//		throw std::exception("XAudio2 could not be instantiated!");
//
//
//	if (FAILED(res = m_xaudio->CreateMasteringVoice(&m_masterVoice)))
//		throw std::exception("MasteringVoice could not be instantiated!");
//
//}
//
//Audio::~Audio()
//{
//	if (m_masterVoice) delete m_masterVoice;
//	if (m_xaudio) m_xaudio->Release();
//}


using DirectX::AUDIO_ENGINE_FLAGS;
using DirectX::AudioEngine;
using DirectX::SoundEffect;
using DirectX::WaveBank;

using DirectX::AudioEngine_Default;
using DirectX::AudioEngine_Debug;

AudioSystem* AudioSystem::a = nullptr;

AudioSystem::AudioSystem()
{
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif
    audioEngine.reset(new AudioEngine(eflags));
    for (auto& p : musicVolume) p = 1.0f;
    for (auto& p : soundVolume) p = 1.0f;
}


AudioSystem* AudioSystem::get()
{
    if (a == nullptr) a = new AudioSystem();   
    return a;
}

AudioSystem::~AudioSystem()
{
    audioEngine.reset();
}

bool AudioSystem::reset()
{
    return audioEngine->Reset();
}

void AudioSystem::musicLoad(int trackNo, const wchar_t* waveFileName, float volume)
{
    if (musicInst[trackNo]) musicInst[trackNo].reset();
    music[trackNo].reset(new SoundEffect(audioEngine.get(), waveFileName));
    musicVolume[trackNo] = volume;
}

void AudioSystem::musicUnload(int trackNo)
{
    if (musicInst[trackNo]) musicInst[trackNo].reset();
    music[trackNo].reset(nullptr);
}

void AudioSystem::musicPlay(int trackNo, bool isLoop)
{
    if (!music[trackNo]) return;
    musicInst[trackNo] = music[trackNo]->CreateInstance();
    musicInst[trackNo]->Play(isLoop);
    musicInst[trackNo]->SetVolume(musicVolume[trackNo]);
}

void AudioSystem::musicStop(int trackNo)
{
    if (!musicInst[trackNo]) return;
    musicInst[trackNo]->Stop();
}

void AudioSystem::musicPause(int trackNo)
{
    if (!musicInst[trackNo]) return;
    musicInst[trackNo]->Pause();
}

void AudioSystem::musicResume(int trackNo)
{
    if (!musicInst[trackNo]) return;
    musicInst[trackNo]->Resume();
}

void AudioSystem::musicSetVolume(int trackNo, float volume)
{
    if (!musicInst[trackNo]) return;
    musicInst[trackNo]->SetVolume(volume);
}

DirectX::SoundState AudioSystem::musicGetState(int trackNo)
{
    return musicInst[trackNo]->GetState();
}

bool AudioSystem::musicIsLooped(int trackNo)
{
    return musicInst[trackNo]->IsLooped();
}

void AudioSystem::musicSetPan(int trackNo, float pan)
{
    return musicInst[trackNo]->SetPan(pan);
}

void AudioSystem::musicSetPitch(int trackNo, float pitch)
{
    return musicInst[trackNo]->SetPitch(pitch);
}

const WAVEFORMATEX* AudioSystem::musicGetFormat(int trackNo)
{
    return music[trackNo]->GetFormat();
}

bool AudioSystem::musicIsInUse(int trackNo)
{
    return music[trackNo]->IsInUse();
}

void AudioSystem::soundLoad(const wchar_t* xwbFileName, float volume)
{
    for (int i = 0; i < WAVE_FILE_MAX; i++)
    {
        for (int j = 0; j < WAVE_SOUND_MAX; j++)
        {
            if (soundInst[i][j])
            {
                soundInst[i][j].reset();
            }
        }
    }

    waveBank.reset(new WaveBank(audioEngine.get(), xwbFileName));

    for (auto& p : soundVolume) p = volume;
}

void AudioSystem::soundPlay(int trackNo)
{
    for (int i = 0; i < WAVE_SOUND_MAX; i++)
    {
        if (soundInst[trackNo][i])
        {
            DirectX::SoundState state = soundInst[trackNo][i]->GetState();
            if (state != DirectX::SoundState::STOPPED) continue;
        }

        soundInst[trackNo][i] = waveBank->CreateInstance(trackNo);
        if (soundInst[trackNo][i])
        {
            soundInst[trackNo][i]->SetVolume(soundVolume[trackNo]);
            soundInst[trackNo][i]->Play();
            break;
        }
    }
}

void AudioSystem::soundSetVolume(int trackNo, float vol)
{
    soundVolume[trackNo] = vol;
}
 

