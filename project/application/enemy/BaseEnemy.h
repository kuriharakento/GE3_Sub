#pragma once
#include <memory>
#include <string>

#include "3d/Object3d.h"

class BaseEnemy
{
public:
	virtual void Initialize(Object3dCommon* object3dCommon, const std::string& filePath) = 0;
	virtual void Update(CameraManager* camera) = 0;
	virtual void Draw() = 0;
protected:
	//3Dオブジェクト
	std::unique_ptr<Object3d> object3d_;
	//トランスフォーム
	Transform transform_;
};

