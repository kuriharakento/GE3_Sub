#pragma once
#include "IParticleBehaviorComponent.h"
#include <cmath>

class OrbitComponent : public IParticleBehaviorComponent
{
public:
    OrbitComponent(const Vector3& c, float radius_, float speed);
    void Update(Particle& particle) override;
private:
    Vector3 center_;
    float angularSpeed_;
    float radius_;
};
