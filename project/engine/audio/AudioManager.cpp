#include "AudioManager.h"

#include <cassert>
#include <fstream>
#include <stdexcept>
#include <sstream>

// インスタンスの初期化
AudioManager* AudioManager::instance_ = nullptr;

AudioManager::AudioManager() {}

AudioManager::~AudioManager() {}

AudioManager* AudioManager::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new AudioManager();
    }
    return instance_;
}

void AudioManager::Initialize()
{
    HRESULT hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create XAudio2 engine.");
    }

    hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create mastering voice.");
    }
}

void AudioManager::Finalize()
{
    for (auto& source : sources) {
        source.second->DestroyVoice();
    }
    if (pMasterVoice) pMasterVoice->DestroyVoice();
    if (pXAudio2) pXAudio2->Release();
    pXAudio2 = nullptr;
    pMasterVoice = nullptr;
    sources.clear();
    sounds.clear();

    delete instance_;
    instance_ = nullptr;
}

void AudioManager::LoadSound(const std::string& name, const std::string& fileName)
{
    std::string fullPath = baseDir + fileName;
    std::ifstream file(fullPath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open sound file: " + fullPath);
    }

    char riffHeader[4];
    file.read(riffHeader, 4);
    if (std::strncmp(riffHeader, "RIFF", 4) != 0) {
        throw std::runtime_error("Invalid WAV file: Missing 'RIFF' header.");
    }

    file.seekg(4, std::ios::cur); // ファイルサイズをスキップ

    char waveHeader[4];
    file.read(waveHeader, 4);
    if (std::strncmp(waveHeader, "WAVE", 4) != 0) {
        throw std::runtime_error("Invalid WAV file: Missing 'WAVE' header.");
    }

    char fmtHeader[4];
    file.read(fmtHeader, 4);
    if (std::strncmp(fmtHeader, "fmt ", 4) != 0) {
        throw std::runtime_error("Invalid WAV file: Missing 'fmt ' chunk.");
    }

    uint32_t fmtChunkSize;
    file.read(reinterpret_cast<char*>(&fmtChunkSize), sizeof(fmtChunkSize));

    WAVEFORMATEX waveFormat;
    file.read(reinterpret_cast<char*>(&waveFormat), sizeof(WAVEFORMATEX));
    if (fmtChunkSize > sizeof(WAVEFORMATEX)) {
        file.seekg(fmtChunkSize - sizeof(WAVEFORMATEX), std::ios::cur);
    }

    char dataHeader[4];
    uint32_t dataSize;
    while (file.read(dataHeader, 4)) {
        file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
        if (std::strncmp(dataHeader, "data", 4) == 0) {
            break;
        }
        file.seekg(dataSize, std::ios::cur);
    }

    if (std::strncmp(dataHeader, "data", 4) != 0) {
        throw std::runtime_error("Invalid WAV file: Missing 'data' chunk.");
    }

    std::vector<BYTE> audioData(dataSize);
    file.read(reinterpret_cast<char*>(audioData.data()), dataSize);

    SoundData soundData;
    soundData.wfx = std::make_unique<WAVEFORMATEX>(waveFormat);
    soundData.bufferData.AudioBytes = dataSize;
    soundData.bufferData.pAudioData = audioData.data();
    soundData.bufferData.Flags = XAUDIO2_END_OF_STREAM;

    sounds[name] = std::move(soundData);
}

void AudioManager::PlaySound(const std::string& name, bool loop)
{
    auto it = sounds.find(name);
    if (it == sounds.end()) {
        throw std::runtime_error("Sound not found: " + name);
    }

    SoundData& soundData = it->second;
    if (sources.find(name) == sources.end()) {
        IXAudio2SourceVoice* pSourceVoice;
        HRESULT hr = pXAudio2->CreateSourceVoice(&pSourceVoice, soundData.wfx.get());
        assert(SUCCEEDED(hr));
        sources[name] = std::unique_ptr<IXAudio2SourceVoice>(pSourceVoice);
    }

    IXAudio2SourceVoice* pSourceVoice = sources[name].get();
    soundData.bufferData.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
    HRESULT hr = pSourceVoice->SubmitSourceBuffer(&soundData.bufferData);
    if (SUCCEEDED(hr)) {
        hr = pSourceVoice->Start();
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to start sound playback.");
        }
    }
}

void AudioManager::StopSound(const std::string& name)
{
    auto it = sources.find(name);
    if (it != sources.end()) {
        it->second->Stop();
    }
}

void AudioManager::SetVolume(const std::string& name, float volume)
{
    auto it = sources.find(name);
    if (it != sources.end()) {
        it->second->SetVolume(volume, XAUDIO2_COMMIT_NOW);
    }
}
