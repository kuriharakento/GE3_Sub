#include "ObstacleManager.h"

void ObstacleManager::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager)
{
	// ポインタをメンバ変数に記録
	object3dCommon_ = object3dCommon;
	lightManager_ = lightManager;

	// リストの初期化
	obstacles_.clear();
}

void ObstacleManager::Update()
{
	for (auto& obstacle : obstacles_)
	{
		if (obstacle)
		{
			obstacle->Update();
		}
	}
}

void ObstacleManager::Draw(CameraManager* camera)
{
	for (auto& obstacle : obstacles_)
	{
		if (obstacle)
		{
			obstacle->Draw(camera);
		}
	}
}


