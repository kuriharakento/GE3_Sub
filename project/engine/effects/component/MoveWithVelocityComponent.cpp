#include "MoveWithVelocityComponent.h"

void MoveWithVelocityComponent::Update(Particle& particle)
{
	particle.transform.translate += particle.velocity;
}