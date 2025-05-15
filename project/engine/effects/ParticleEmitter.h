#pragma once
#include <memory>

#include "ParticleManager.h"

class ParticleEmitter
{
public:
	ParticleEmitter(Transform transform,uint32_t count);

	void Update();

	void Emit();

private:
	//座標
	Transform transform_ = {};

	//時間
	uint32_t time_ = 0;

	//パーティクルの数
	uint32_t count_ = 5;

	//発生頻度
	const uint32_t kEmitFrequency_ = 60;

	//グループ番号
	uint32_t groupIndex_ = 1;

	//モデルのファイルパス
	std::string modelFilePath_ = "Resources/axis.obj";

};

