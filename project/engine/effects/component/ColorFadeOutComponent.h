#pragma once
#include "IParticleBehaviorComponent.h"

class ColorFadeOutComponent : public IParticleBehaviorComponent
{
public:
    void Update(Particle& particle) override;
};
