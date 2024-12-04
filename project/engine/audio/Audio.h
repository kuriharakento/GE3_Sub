#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>
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
	void Initialize();

	SoundData LoadWave(const char* filename);

private:
	//IXAudio2
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	//IXAudio2MasteringVoice
	IXAudio2MasteringVoice* masterVoice;
};

