#include "MoveWithVelocityComponent.h"

void MoveWithVelocityComponent::Update(Particle& particle)
{
	particle.transform.translate += particle.velocity * (1.0f / 60.0f); // 1フレーム分の時間を加算
}