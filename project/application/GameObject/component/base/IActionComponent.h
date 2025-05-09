#pragma once
#include "application/GameObject/base/GameObject.h"

class IActionComponent : public IGameObjectComponent {
public:
    virtual void Update(GameObject* owner) = 0;
	virtual void Draw(CameraManager* camera){}
};
