#include "Audio.h"

#include <cstring>
#include <cassert>

Audio* Audio::instance_ = nullptr;

Audio* Audio::GetInstance()
{
	if (!instance_)
	{
		instance_ = new Audio();
	}
	return instance_;
}

void Audio::Initialize()
{
	//XAudio2エンジンのインスタンスを生成
	XAudio2Create(&xAudio2,0,XAUDIO2_DEFAULT_PROCESSOR);
	//マスターボイスを生成
	xAudio2->CreateMasteringVoice(&masterVoice);
}

void Audio::Finalize()
{
	// 全ての音声データを解放
	for (auto& pair : soundDataMap_)
	{
		if (pair.second.pBuffer)
		{
			delete[] pair.second.pBuffer;
			pair.second.pBuffer = nullptr;
		}
	}
	soundDataMap_.clear();

	// 再生中のソースボイスを解放
	for (auto& pair : sourceVoiceMap_)
	{
		if (pair.second)
		{
			pair.second->Stop(0);
			pair.second->FlushSourceBuffers();
			pair.second->DestroyVoice();
		}
	}
	sourceVoiceMap_.clear();

	//マスターボイスを破棄
	masterVoice->DestroyVoice();
	masterVoice = nullptr;
	//XAudio2エンジンを破棄
	xAudio2->StopEngine();
	xAudio2.Reset();

	//シングルトンの解放
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

SoundData Audio::LoadWave(const char* filename)
{
	/*--------------[ ファイルの読み込み ]-----------------*/

	//ファイル入力ストリームのインスタンスを生成
	std::ifstream file;
	//バイナリモードでファイルを開く
	file.open(filename, std::ios::binary);
	//ファイルが開けなかった場合
	assert(file.is_open());

	/*--------------[ wavデータ読み込み ]-----------------*/

	//RIFFヘッダの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0)
	{
		assert(0);
	}

	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0)
	{
		//JUNKチャンクをスキップ
		file.seekg(data.size, std::ios::cur);
		//次のチャンクを読み込む
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	//Dataチャンクのデータ部分を読み込む
	char* buffer = new char[data.size];
	file.read(buffer, data.size);

	//waveファイルを閉じる
	file.close();

	/*--------------[ 読み込んだ音声データをreturn ]-----------------*/

	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(buffer);
	soundData.bufferSize = data.size;

	return soundData;
}

void Audio::LoadWave(const std::string& name, const char* filename)
{
	// ファイル入力ストリームのインスタンスを生成
	std::ifstream file;
	// バイナリモードでファイルを開く
	file.open(filename, std::ios::binary);
	// ファイルが開けなかった場合
	assert(file.is_open());

	// RIFFヘッダの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	assert(strncmp(riff.chunk.id, "RIFF", 4) == 0);
	// タイプがWAVEかチェック
	assert(strncmp(riff.type, "WAVE", 4) == 0);

	// 'fmt 'チャンクと'data'チャンクを見つけるまでループ
	FormatChunk format = {};
	BYTE* buffer = nullptr;
	unsigned int dataSize = 0;

	while (file.peek() != EOF)
	{
		// チャンクヘッダの読み込み
		ChunkHeader chunkHeader;
		file.read((char*)&chunkHeader, sizeof(chunkHeader));

		// チャンクIDに応じて処理
		if (strncmp(chunkHeader.id, "fmt ", 4) == 0)
		{
			// 'fmt 'チャンクの読み込み
			assert(chunkHeader.size <= sizeof(format.fmt));
			file.read((char*)&format.fmt, chunkHeader.size);
		} else if (strncmp(chunkHeader.id, "data", 4) == 0)
		{
			// 'data'チャンクの読み込み
			buffer = new BYTE[chunkHeader.size];
			dataSize = chunkHeader.size;
			file.read(reinterpret_cast<char*>(buffer), chunkHeader.size);
		} else
		{
			// 他のチャンクは読み飛ばす
			file.seekg(chunkHeader.size, std::ios::cur);
		}

		// 必要なチャンクが見つかったら終了
		if (format.fmt.wFormatTag && buffer)
		{
			break;
		}
	}

	// チャンクが見つかったか確認
	assert(format.fmt.wFormatTag != 0);
	assert(buffer != nullptr);

	// ファイルを閉じる
	file.close();

	// 読み込んだ音声データをマップに保存
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = buffer;
	soundData.bufferSize = dataSize;

	soundDataMap_[name] = soundData;
}

void Audio::PlayWave(SoundData* soundData, bool loop)
{
	HRESULT result;

	//波形フォーマットをもとにソースボイスを生成
	IXAudio2SourceVoice* sourceVoice;
	result = xAudio2->CreateSourceVoice(&sourceVoice, &soundData->wfex);
	assert(SUCCEEDED(result));

	//再生する波形データをセット
	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = soundData->bufferSize;
	buffer.pAudioData = soundData->pBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;	//無限ループ

	//波形データの再生
	result = sourceVoice->SubmitSourceBuffer(&buffer);
	result = sourceVoice->Start();
}

void Audio::PlayWave(const std::string& name, bool loop)
{
	auto it = soundDataMap_.find(name);
	if (it == soundDataMap_.end())
	{
		return;
	}

	SoundData& soundData = it->second;

	HRESULT hr;

	// ソースボイスを生成
	IXAudio2SourceVoice* sourceVoice = nullptr;
	hr = xAudio2->CreateSourceVoice(&sourceVoice, &soundData.wfex);
	assert(SUCCEEDED(hr));

	// 再生する波形データをセット
	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = soundData.bufferSize;
	buffer.pAudioData = soundData.pBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	if (loop)
	{
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE; // 無限ループ
	}

	// 波形データの再生
	hr = sourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hr));

	hr = sourceVoice->Start(0);
	assert(SUCCEEDED(hr));

	// ソースボイスをマップに保存
	sourceVoiceMap_[name] = sourceVoice;
}

void Audio::StopWave(const std::string& name)
{
	auto it = sourceVoiceMap_.find(name);
	if (it != sourceVoiceMap_.end())
	{
		IXAudio2SourceVoice* sourceVoice = it->second;
		sourceVoice->Stop(0);
		sourceVoice->FlushSourceBuffers();
		sourceVoice->DestroyVoice();
		sourceVoiceMap_.erase(it);
	}
}

void Audio::SetVolume(const std::string& name, float volume)
{
	auto it = sourceVoiceMap_.find(name);
	if (it != sourceVoiceMap_.end())
	{
		IXAudio2SourceVoice* sourceVoice = it->second;
		// 音量を設定（0.0f～1.0f）
		sourceVoice->SetVolume(volume);
	}
}
