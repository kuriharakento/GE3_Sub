#pragma once
#include "effects/component/interface/IParticleBehaviorComponent.h"

class ColorFadeOutComponent : public IParticleBehaviorComponent
{
public:
    void Update(Particle& particle) override;
};
