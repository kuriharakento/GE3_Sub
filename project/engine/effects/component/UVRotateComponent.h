#pragma once
#include "IParticleGroupComponent.h"
#include "effects/ParticleGroup.h"

class UVRotateComponent : public IParticleGroupComponent
{
public:
    explicit UVRotateComponent(const Vector3& rotate);

    void Update(ParticleGroup& group) override;

private:
    Vector3 rotate_;
};
