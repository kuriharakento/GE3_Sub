#pragma once
#include "IGameObjectComponent.h"

class ICollisionComponent : public virtual IGameObjectComponent
{
	virtual ~ICollisionComponent() = default;
};