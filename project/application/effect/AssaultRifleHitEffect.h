#pragma once
#include "effects/ParticleEmitter.h"

class AssaultRifleHitEffect
{
public:
	void Initialize();

	void Play(const Vector3& position);

private:
	// エミッター
	std::unique_ptr<ParticleEmitter> emitter_;
};

