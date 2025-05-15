#pragma once
#include "effects/ParticleGroup.h"
#include "effects/component/interface/IParticleGroupComponent.h"

class UVRotateComponent : public IParticleGroupComponent
{
public:
    explicit UVRotateComponent(const Vector3& rotate);

    void Update(ParticleGroup& group) override;

private:
    Vector3 rotate_;
};
