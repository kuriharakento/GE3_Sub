#pragma once
#include "3d/Object3d.h"
class CameraManager;

class Skydome
{
public:
	void Initialize(Object3dCommon* object3dCommon, const std::string& modelName);
	void Update(CameraManager* cameraManager);
	void Draw();

private:
	std::unique_ptr<Object3d> skydome_;
	Transform transform_;
};

