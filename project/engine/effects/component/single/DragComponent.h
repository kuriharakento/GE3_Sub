#pragma once
#include "effects/component/interface/IParticleBehaviorComponent.h"

class DragComponent : public IParticleBehaviorComponent
{
public:
    explicit DragComponent(float drag);
    void Update(Particle& particle) override;
private:
    float dragFactor_;
};
