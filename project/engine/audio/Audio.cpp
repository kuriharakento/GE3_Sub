//#include "Audio.h"
//
//#include <cassert>
//
//void Audio::Initialize()
//{
//	//XAudio2エンジンのインスタンスを生成
//	XAudio2Create(&xAudio2,0,XAUDIO2_DEFAULT_PROCESSOR);
//	//マスターボイスを生成
//	xAudio2->CreateMasteringVoice(&masterVoice);
//}
//
//SoundData Audio::LoadWave(const char* filename)
//{
//	HRESULT result;
//
//	/*--------------[ ファイルの読み込み ]-----------------*/
//
//	//ファイル入力ストリームのインスタンスを生成
//	std::ifstream file;
//	//バイナリモードでファイルを開く
//	file.open(filename, std::ios::binary);
//	//ファイルが開けなかった場合
//	assert(file.is_open());
//
//	/*--------------[ wavデータ読み込み ]-----------------*/
//
//	//RIFFヘッダの読み込み
//	RiffHeader riff;
//	file.read((char*)&riff, sizeof(riff));
//	//ファイルがRIFFかチェック
//	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
//	{
//		assert(0);
//	}
//	//タイプがWAVEかチェック
//	if (strncmp(riff.type, "WAVE", 4) != 0)
//	{
//		assert(0);
//	}
//
//	//Formatチャンクの読み込み
//	FormatChunk format = {};
//	//チャンクヘッダーの確認
//	file.read((char*)&format, sizeof(ChunkHeader));
//	if (strncmp(format.chunk.id, "fmt ", 4) != 0)
//	{
//		assert(0);
//	}
//
//	//チャンク本体の読み込み
//	assert(format.chunk.size <= sizeof(format.fmt));
//	file.read((char*)&format.fmt, format.chunk.size);
//
//	//Dataチャンクの読み込み
//	ChunkHeader data;
//	file.read((char*)&data, sizeof(data));
//	//JUNKチャンクを検出した場合
//	if (strncmp(data.id, "JUNK", 4) == 0)
//	{
//		//JUNKチャンクをスキップ
//		file.seekg(data.size, std::ios::cur);
//		//次のチャンクを読み込む
//		file.read((char*)&data, sizeof(data));
//	}
//
//	if (strncmp(data.id, "data", 4) != 0)
//	{
//		assert(0);
//	}
//
//	//Dataチャンクのデータ部分を読み込む
//	char* buffer = new char[data.size];
//	file.read(buffer, data.size);
//
//	//waveファイルを閉じる
//	file.close();
//
//	/*--------------[ 読み込んだ音声データをreturn ]-----------------*/
//
//	SoundData soundData = {};
//	soundData.wfex = format.fmt;
//	soundData.pBuffer = reinterpret_cast<BYTE*>(buffer);
//	soundData.bufferSize = data.size;
//
//	return soundData;
//}
//
//void Audio::SoundUnload(SoundData* soundData)
//{
//	delete[] soundData->pBuffer;
//
//	soundData->pBuffer = nullptr;
//	soundData->bufferSize = 0;
//	soundData->wfex = {};
//
//}
