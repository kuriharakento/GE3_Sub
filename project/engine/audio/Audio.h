#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>
#include <unordered_map>
#include <wrl.h>

//チャンクヘッダ
struct ChunkHeader
{
	char id[4];					//チャンクID
	int32_t size;				//チャンクサイズ
};

//RIFFヘッダ
struct RiffHeader
{
	ChunkHeader chunk;			//RIFF
	char type[4];				//WAVE
};

//FMTチャンク
struct FormatChunk
{
	ChunkHeader chunk;
	WAVEFORMATEX fmt;			//波形フォーマット
};

//音声データ
struct SoundData
{
	WAVEFORMATEX wfex;			//波形フォーマット
	BYTE* pBuffer;				//バッファの先頭アドレス
	unsigned int bufferSize;	//バッファのサイズ
};

class Audio
{
public:
	//シングルトン
	static Audio* GetInstance();
	//初期化
	void Initialize();
	//終了
	void Finalize();
	//音声データの読み込み
	SoundData LoadWave(const char* filename);
	void LoadWave(const std::string& name, const char* filename);
	//再生
	void PlayWave(SoundData* soundData, bool loop = false);
	void PlayWave(const std::string& name, bool loop = false);
	//解放
	void SoundUnload(SoundData* soundData);

private:
	//IXAudio2
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	//IXAudio2MasteringVoice
	IXAudio2MasteringVoice* masterVoice;

	// 音声データのマップ
	std::unordered_map<std::string, SoundData> soundDataMap_;

private: //シングルトン
	static Audio* instance_;

	Audio() {}
	~Audio() {}
	Audio(const Audio&) = delete;
	Audio& operator=(const Audio&) = delete;
};

