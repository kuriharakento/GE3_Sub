#pragma once
#include "effects/component/interface/IParticleBehaviorComponent.h"

class AccelerationComponent : public IParticleBehaviorComponent
{
public:
    explicit AccelerationComponent(const Vector3& accel);
    void Update(Particle& particle) override;
private:
    Vector3 acceleration_;
};
