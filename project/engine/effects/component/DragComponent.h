#pragma once
#include "IParticleBehaviorComponent.h"

class DragComponent : public IParticleBehaviorComponent
{
public:
    explicit DragComponent(float drag);
    void Update(Particle& particle) override;
private:
    float dragFactor_;
};
