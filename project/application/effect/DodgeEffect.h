#pragma once
#include "effects/ParticleEmitter.h"
#include "math/Vector3.h"

class DodgeEffect
{
public:
	void Initialize();
	void Play(const Vector3& position);

private:
	std::unique_ptr<ParticleEmitter> emitter_;
};

