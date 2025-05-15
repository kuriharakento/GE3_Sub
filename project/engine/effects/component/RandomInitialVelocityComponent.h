#pragma once
#include "IParticleBehaviorComponent.h"
#include <cstdlib>

class RandomInitialVelocityComponent : public IParticleBehaviorComponent
{
public:
    RandomInitialVelocityComponent(const Vector3& minV, const Vector3& maxV);
    void Update(Particle& particle) override;

private:
    float RandomFloat(float min, float max);
    Vector3 minVelocity_;
    Vector3 maxVelocity_;
    bool initialized_ = false;
};
