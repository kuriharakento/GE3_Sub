#pragma once
#include "IParticleBehaviorComponent.h"

class MoveWithVelocityComponent : public IParticleBehaviorComponent
{
public:
    void Update(Particle& particle) override;
};
