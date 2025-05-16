#pragma once
#include <cmath>

#include "effects/component/interface/IParticleBehaviorComponent.h"

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
