#include "Obstacle.h"

void Obstacle::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager)
{
	// ゲームオブジェクトの初期化
	GameObject::Initialize(object3dCommon, lightManager);
}

void Obstacle::Update()
{
	// ゲームオブジェクトの更新
	GameObject::Update();
}

void Obstacle::Draw(CameraManager* camera)
{
	// ゲームオブジェクトの描画処理
	GameObject::Draw(camera);
}
