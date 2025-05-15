#pragma once
#include "IParticleGroupComponent.h"
#include "effects/ParticleGroup.h"
#include "math/Vector3.h"

class UVScaleComponent : public IParticleGroupComponent
{
public:
    explicit UVScaleComponent(const Vector3& scale);

    void Update(ParticleGroup& group) override;

private:
    Vector3 scale_;
};
