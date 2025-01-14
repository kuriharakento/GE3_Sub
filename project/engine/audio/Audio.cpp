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
	// フェードリストをクリア
	fadeList_.clear();

	// 再生中のすべてのソースボイスを停止および破棄
	for (auto& pair : sourceVoiceMap_)
	{
		IXAudio2SourceVoice* sourceVoice = pair.second;
		if (sourceVoice)
		{
			sourceVoice->Stop(0);
			sourceVoice->FlushSourceBuffers();
			sourceVoice->DestroyVoice();
		}
	}
	sourceVoiceMap_.clear();

	// グループごとのソースボイスリストをクリア
	groupVoicesMap_.clear();

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

// Audio::Update 内のフェード処理を修正
void Audio::Update()
{
	// 1フレームあたりの時間
	const float deltaTime = 1.0f / 60.0f;

	// フェード処理の更新
	for (auto it = fadeList_.begin(); it != fadeList_.end(); )
	{
		FadeData& fadeData = *it;
		if (fadeData.isFading && fadeData.sourceVoice)
		{
			fadeData.currentTime += deltaTime;
			float t = fadeData.currentTime / fadeData.duration;
			if (t >= 1.0f)
			{
				t = 1.0f;
				fadeData.isFading = false;
			}
			// 線形補間で音量を計算
			float volume = fadeData.startVolume + (fadeData.targetVolume - fadeData.startVolume) * t;
			fadeData.sourceVoice->SetVolume(volume);

			// フェードアウト完了後に音声を停止
			if (!fadeData.isFading && fadeData.targetVolume == 0.0f)
			{
				fadeData.sourceVoice->Stop(0);
				// sourceVoiceMap_から削除
				for (auto mapIt = sourceVoiceMap_.begin(); mapIt != sourceVoiceMap_.end(); ++mapIt)
				{
					if (mapIt->second == fadeData.sourceVoice)
					{
						sourceVoiceMap_.erase(mapIt);
						break;
					}
				}
			}

			if (!fadeData.isFading)
			{
				it = fadeList_.erase(it);
				continue;
			}
		}
		++it;
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

void Audio::LoadWave(const std::string& name, const char* filename, SoundGroup group)
{
	//フルパス
	std::string fullpath = directoryPath + std::string(filename);
	// ファイル入力ストリームのインスタンスを生成
	std::ifstream file;
	// バイナリモードでファイルを開く
	file.open(fullpath, std::ios::binary);
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
	soundData.group = group; // グループ情報を設定

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
	if (it == soundDataMap_.end()) {
		return;
	}
	SoundData& soundData = it->second;

	HRESULT hr;
	IXAudio2SourceVoice* sourceVoice = nullptr;
	hr = xAudio2->CreateSourceVoice(&sourceVoice, &soundData.wfex);
	assert(SUCCEEDED(hr));

	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = soundData.bufferSize;
	buffer.pAudioData = soundData.pBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

	hr = sourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hr));

	hr = sourceVoice->Start(0);
	assert(SUCCEEDED(hr));

	sourceVoiceMap_[name] = sourceVoice;
	groupVoicesMap_[soundData.group].push_back(sourceVoice);
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

void Audio::StopGroup(SoundGroup group)
{
	auto it = groupVoicesMap_.find(group);
	if (it != groupVoicesMap_.end())
	{
		for (auto& sourceVoice : it->second)
		{
			sourceVoice->Stop(0);
			sourceVoice->FlushSourceBuffers();
			sourceVoice->DestroyVoice();
		}
		it->second.clear();
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

void Audio::SetGroupVolume(SoundGroup group, float volume)
{
	auto it = groupVoicesMap_.find(group);
	if (it != groupVoicesMap_.end())
	{
		for (auto& sourceVoice : it->second)
		{
			sourceVoice->SetVolume(volume);
		}
	}
}

void Audio::FadeIn(const std::string& name, float duration)
{
	// 既存のソースボイスを検索
	auto it = sourceVoiceMap_.find(name);
	IXAudio2SourceVoice* sourceVoice = nullptr;

	if (it != sourceVoiceMap_.end())
	{
		sourceVoice = it->second;
	} else
	{
		// ソースボイスが存在しない場合は新たに作成
		auto soundIt = soundDataMap_.find(name);
		if (soundIt == soundDataMap_.end()) {
			return;
		}

		SoundData& soundData = soundIt->second;
		HRESULT hr = xAudio2->CreateSourceVoice(&sourceVoice, &soundData.wfex);
		if (FAILED(hr)) {
			// エラーハンドリング
			return;
		}

		XAUDIO2_BUFFER buffer = {};
		buffer.AudioBytes = soundData.bufferSize;
		buffer.pAudioData = soundData.pBuffer;
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

		hr = sourceVoice->SubmitSourceBuffer(&buffer);
		if (FAILED(hr)) {
			// エラーハンドリング
			sourceVoice->DestroyVoice();
			return;
		}

		hr = sourceVoice->Start(0);
		if (FAILED(hr)) {
			// エラーハンドリング
			sourceVoice->DestroyVoice();
			return;
		}

		// マップに追加
		sourceVoiceMap_[name] = sourceVoice;
		groupVoicesMap_[soundData.group].push_back(sourceVoice);
	}

	// フェードデータを追加
	FadeData fadeData = {};
	fadeData.sourceVoice = sourceVoice;
	fadeData.startVolume = 0.0f;
	fadeData.targetVolume = 1.0f;
	fadeData.currentTime = 0.0f;
	fadeData.duration = duration;
	fadeData.isFading = true;
	sourceVoice->SetVolume(0.0f);
	fadeList_.push_back(fadeData);
}

void Audio::FadeOut(const std::string& name, float duration)
{
	auto it = sourceVoiceMap_.find(name);
	if (it != sourceVoiceMap_.end())
	{
		IXAudio2SourceVoice* sourceVoice = it->second;
		// フェードデータを追加
		FadeData fadeData = {};
		fadeData.sourceVoice = sourceVoice;
		fadeData.startVolume = 1.0f;
		fadeData.targetVolume = 0.0f;
		fadeData.currentTime = 0.0f;
		fadeData.duration = duration;
		fadeData.isFading = true;
		fadeList_.push_back(fadeData);
	}
}
