#pragma once
#include "application/Entities/Building.h"

#include <vector>
#include <memory>

class CameraManager;
class Object3dCommon;

class BuildingManager
{
public:
	void Initialize(Object3dCommon* objectCommon, CameraManager* camera);

	void Update();

	void Draw();

	void GenerateBuilding(int count);

private:
	//ポインタ
	Object3dCommon* objectCommon_;
	CameraManager* cameraManager_;

	// 建物リスト
	std::vector<std::unique_ptr<Building>> buildings_;

	// テクスチャのファイルパス
	std::string filePath_ = "axis.obj";
};

