#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(Transform transform, uint32_t count)
{
	transform_ = transform;
	count_ = count;
}

void ParticleEmitter::Update()
{
	//経過時間を加算
	time_++;
	//一定時間経過したらパーティクルを発生させる
	if(time_ >= kEmitFrequency_)
	{
		Emit();
		time_ = 0;
	}
}

void ParticleEmitter::Emit()
{
	std::string groupName = std::to_string(groupIndex_);
	ParticleManager::GetInstance()->Emit(groupName,transform_.translate,count_);
	groupIndex_++;
}
