#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <unordered_map>
#include <memory>
#include <string>

// サウンドデータ構造体
struct SoundData {
    std::unique_ptr<WAVEFORMATEX> wfx;
    XAUDIO2_BUFFER bufferData = {};
    std::vector<BYTE> buffer;
};

class AudioManager {
public:
    static AudioManager* GetInstance();

	// 初期化と終了処理
	void Initialize();
	void Finalize();

	// サウンドの読み込み
    void LoadSound(const std::string& name, const std::string& fileName);
	// サウンドの再生
	void PlaySoundW(const std::string& name, bool loop = false);
	// サウンドの停止
	void StopSound(const std::string& name);
	// サウンドのボリューム調整
	void SetVolume(const std::string& name, float volume);

private:
    // XAudio2関連
    IXAudio2* pXAudio2 = nullptr;
    IXAudio2MasteringVoice* pMasterVoice = nullptr;

    // サウンドとソースのマップ
    std::unordered_map<std::string, SoundData> sounds;
    std::unordered_map<std::string, std::unique_ptr<IXAudio2SourceVoice>> sources;

    //ディレクトリパス
	const std::string baseDir = "./Resources/audio/";

private: //シングルトンインスタンス
	static AudioManager* instance_;
    //コピー禁止
    AudioManager();
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

};