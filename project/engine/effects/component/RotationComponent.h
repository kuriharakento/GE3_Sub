#pragma once
#include "IParticleBehaviorComponent.h"
#include "base/GraphicsTypes.h"

class RotationComponent : public IParticleBehaviorComponent
{
public:
    explicit RotationComponent(const Vector3& rotSpeed);
    void Update(Particle& particle) override;
private:
    Vector3 rotationSpeed_;
};
