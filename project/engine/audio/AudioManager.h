#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <xaudio2.h>
#include <cstring>

// 音声情報を管理する構造体
struct SoundData {
    std::vector<BYTE> audioData;
    WAVEFORMATEX* waveFormat;
    IXAudio2SourceVoice* sourceVoice;
    float volume;

    SoundData() : waveFormat(nullptr), sourceVoice(nullptr), volume(1.0f) {}
};

// WAV ファイルのヘッダー構造体
struct WAVHeader {
    char riff[4];                // "RIFF"
    uint32_t chunkSize;          // 36 + SubChunk2Size
    char wave[4];                // "WAVE"
    char fmt[4];                 // "fmt "
    uint32_t subChunk1Size;      // 16 for PCM
    uint16_t audioFormat;        // PCM = 1
    uint16_t numChannels;        // Mono = 1, Stereo = 2
    uint32_t sampleRate;         // 44100, 48000, etc.
    uint32_t byteRate;           // SampleRate * NumChannels * BitsPerSample/8
    uint16_t blockAlign;         // NumChannels * BitsPerSample/8
    uint16_t bitsPerSample;      // 16, 24, etc.
    char data[4];                // "data"
    uint32_t dataSize;           // NumSamples * NumChannels * BitsPerSample/8
};

class AudioManager {
public:
    static AudioManager* GetInstance();
    bool Initialize();
    void Finalize();

    void LoadWavFile(const std::wstring& fileName, const std::wstring& filePath);
    void PlayMusic(const std::wstring& fileName, bool loop);
    void StopMusic(const std::wstring& fileName);
    bool IsMusicPlaying(const std::wstring& fileName) const;

    // 各音声の音量調整用メソッドを追加
    void SetVolume(const std::wstring& fileName, float volume); // volume: 0.0f (ミュート) ~ 1.0f (最大音量)
    float GetVolume(const std::wstring& fileName) const;

private:
    AudioManager();
    ~AudioManager();

    static AudioManager* instance;

    IXAudio2* pXAudio2_;
    IXAudio2MasteringVoice* pMasterVoice_;

    std::wstring basePath_ = L"./Resources/audio/";
    std::unordered_map<std::wstring, SoundData> soundMap_;
};
