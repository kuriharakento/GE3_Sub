#pragma once
#include "base/GraphicsTypes.h"

class IParticleBehaviorComponent
{
public:
	virtual ~IParticleBehaviorComponent() = default;
	virtual void Update(Particle& particle) = 0;
};
