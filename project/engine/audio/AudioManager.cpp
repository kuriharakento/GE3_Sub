// AudioManager.cpp

#include "AudioManager.h"
#include <fstream>
#include <sstream>
#include <cstring> // for std::strncmp
#include "base/Logger.h"

AudioManager* AudioManager::instance = nullptr;

AudioManager::AudioManager() : pXAudio2_(nullptr), pMasterVoice_(nullptr) {}

AudioManager* AudioManager::GetInstance() {
    if (!instance) {
        instance = new AudioManager();
    }
    return instance;
}

AudioManager::~AudioManager() {
    Finalize();
}

bool AudioManager::Initialize() {
    HRESULT hr;

    // XAudio2の初期化
    hr = XAudio2Create(&pXAudio2_, 0);
    if (FAILED(hr)) {
        Logger::Log("Failed to initialize XAudio2.");
        return false;
    }

    // マスターボイスの作成
    hr = pXAudio2_->CreateMasteringVoice(&pMasterVoice_);
    if (FAILED(hr)) {
        Logger::Log("Failed to create mastering voice.");
        return false;
    }

    Logger::Log("XAudio2 initialized successfully.");
    return true;
}

void AudioManager::Finalize() {
    // 全てのSourceVoiceを停止して破棄
    for (auto& [name, sound] : soundMap_) {
        if (sound.sourceVoice) {
            sound.sourceVoice->Stop();
            sound.sourceVoice->DestroyVoice();
            sound.sourceVoice = nullptr;
        }
        // WAVEFORMATEX の解放
        if (sound.waveFormat) {
            delete sound.waveFormat;
            sound.waveFormat = nullptr;
        }
    }
    soundMap_.clear();

    // MasteringVoice を破棄
    if (pMasterVoice_) {
        pMasterVoice_->DestroyVoice();
        pMasterVoice_ = nullptr;
    }

    // XAudio2 エンジンを停止して解放
    if (pXAudio2_) {
        pXAudio2_->StopEngine();
        pXAudio2_->Release();
        pXAudio2_ = nullptr;
    }

    // シングルトンインスタンスを解除
    instance = nullptr;

    Logger::Log("AudioManager finalized successfully.");
}

void AudioManager::LoadWavFile(const std::wstring& fileName, const std::wstring& filePath) {
    // すでにロードされているか確認
    if (soundMap_.find(fileName) != soundMap_.end()) {
        Logger::Log(L"Audio file already loaded: " + fileName);
        return; // 早期リターン
    }

    // フルパスを作成
    std::wstring fullPath = basePath_ + filePath;

    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        Logger::Log(L"Failed to open WAV file: " + fullPath);
        return;
    }

    // ヘッダーの読み込み
    WAVHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));

    // ヘッダーの検証
    if (std::strncmp(header.riff, "RIFF", 4) != 0 ||
        std::strncmp(header.wave, "WAVE", 4) != 0 ||
        std::strncmp(header.fmt, "fmt ", 4) != 0 ||
        std::strncmp(header.data, "data", 4) != 0) {
        Logger::Log(L"Invalid WAV file format: " + fullPath);
        file.close();
        return;
    }

    if (header.audioFormat != 1) { // PCM 形式か確認
        Logger::Log(L"Unsupported audio format (Not PCM): " + fullPath);
        file.close();
        return;
    }

    // 音声データの読み込み
    std::vector<BYTE> audioData(header.dataSize);
    file.read(reinterpret_cast<char*>(audioData.data()), header.dataSize);
    file.close();

    // マップに保存
    SoundData sound;
    sound.audioData = std::move(audioData);

    // WAVEFORMATEX の構築
    WAVEFORMATEX* waveFormat = new WAVEFORMATEX();
    waveFormat->wFormatTag = header.audioFormat;
    waveFormat->nChannels = header.numChannels;
    waveFormat->nSamplesPerSec = header.sampleRate;
    waveFormat->nAvgBytesPerSec = header.byteRate;
    waveFormat->nBlockAlign = header.blockAlign;
    waveFormat->wBitsPerSample = header.bitsPerSample;
    waveFormat->cbSize = 0;

    sound.waveFormat = waveFormat;
    sound.volume = 1.0f; // デフォルト音量

    soundMap_[fileName] = std::move(sound);

    Logger::Log(L"Successfully loaded WAV file: " + fileName);
}

void AudioManager::PlayMusic(const std::wstring& fileName, bool loop) {
    // 音声データが事前にロードされているか確認
    auto it = soundMap_.find(fileName);
    if (it == soundMap_.end()) {
        Logger::Log(L"Audio data not loaded for file: " + fileName);
        return;
    }

    SoundData& sound = it->second;

    if (sound.sourceVoice) {
        Logger::Log(L"SourceVoice already exists for: " + fileName);
        return;
    }

    // SourceVoice の作成
    HRESULT hr = pXAudio2_->CreateSourceVoice(&sound.sourceVoice, sound.waveFormat);
    if (FAILED(hr)) {
        Logger::Log("Failed to create source voice.");
        return;
    }

    // バッファの作成と再生
    XAUDIO2_BUFFER audioBuffer = { 0 };
    audioBuffer.AudioBytes = static_cast<UINT32>(sound.audioData.size());
    audioBuffer.pAudioData = sound.audioData.data();
    audioBuffer.Flags = loop ? XAUDIO2_LOOP_INFINITE : XAUDIO2_END_OF_STREAM;

    hr = sound.sourceVoice->SubmitSourceBuffer(&audioBuffer);
    if (FAILED(hr)) {
        Logger::Log("Failed to submit source buffer.");
        return;
    }

    // 音量を設定
    hr = sound.sourceVoice->SetVolume(sound.volume);
    if (FAILED(hr)) {
        Logger::Log("Failed to set volume.");
    }

    // 再生開始
    hr = sound.sourceVoice->Start(0);
    if (FAILED(hr)) {
        Logger::Log("Failed to start audio.");
    } else {
        Logger::Log(L"Started playback for: " + fileName);
    }
}

void AudioManager::StopMusic(const std::wstring& fileName) {
    auto it = soundMap_.find(fileName);
    if (it == soundMap_.end()) {
        Logger::Log(L"Audio data not loaded for file: " + fileName);
        return;
    }

    SoundData& sound = it->second;

    if (sound.sourceVoice) {
        sound.sourceVoice->Stop();
        sound.sourceVoice->FlushSourceBuffers();
        sound.sourceVoice->DestroyVoice();
        sound.sourceVoice = nullptr;
        Logger::Log(L"Stopped playback for: " + fileName);
    }
}

bool AudioManager::IsMusicPlaying(const std::wstring& fileName) const {
    auto it = soundMap_.find(fileName);
    if (it == soundMap_.end()) {
        return false;
    }

    const SoundData& sound = it->second;

    if (sound.sourceVoice) {
        XAUDIO2_VOICE_STATE state;
        sound.sourceVoice->GetState(&state);
        return state.BuffersQueued > 0;
    }
    return false;
}

// 音量を設定するメソッドの実装
void AudioManager::SetVolume(const std::wstring& fileName, float volume) {
    auto it = soundMap_.find(fileName);
    if (it == soundMap_.end()) {
        Logger::Log(L"Audio data not loaded for file: " + fileName);
        return;
    }

    SoundData& sound = it->second;

    // 音量を範囲内にクランプ
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;

    sound.volume = volume;

    if (sound.sourceVoice) {
        HRESULT hr = sound.sourceVoice->SetVolume(sound.volume);
        if (FAILED(hr)) {
            Logger::Log("Failed to set volume.");
        } else {
            Logger::Log(L"Volume set to " + std::to_wstring(sound.volume) + L" for: " + fileName);
        }
    }
}

float AudioManager::GetVolume(const std::wstring& fileName) const {
    auto it = soundMap_.find(fileName);
    if (it == soundMap_.end()) {
        Logger::Log(L"Audio data not loaded for file: " + fileName);
        return 0.0f;
    }

    const SoundData& sound = it->second;
    return sound.volume;
}
