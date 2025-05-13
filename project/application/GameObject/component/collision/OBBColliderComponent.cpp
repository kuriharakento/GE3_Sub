#include "OBBColliderComponent.h"
#include "application/GameObject/base/GameObject.h"

OBBColliderComponent::OBBColliderComponent(GameObject* owner) : ICollisionComponent(owner)
{
	// OBBの初期化
	obb_.center = owner->GetPosition();
	obb_.rotate = MakeRotateMatrix(owner->GetRotation());
	obb_.size = owner->GetScale();
}

void OBBColliderComponent::Update(GameObject* owner)
{
	// オーナーの位置とスケールを取得
	Vector3 pos = owner->GetPosition();
	Vector3 rotate = owner->GetRotation();
	Vector3 size = owner->GetScale();
	// OBBの更新
	obb_.center = pos;
	obb_.rotate = MakeRotateMatrix(rotate);
	obb_.size = size;
}
