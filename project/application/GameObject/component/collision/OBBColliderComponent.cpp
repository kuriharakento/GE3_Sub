#include "OBBColliderComponent.h"
#include "application/GameObject/base/GameObject.h"
#include "lighting/VectorColorCodes.h"
#include "line/LineManager.h"

OBBColliderComponent::OBBColliderComponent(GameObject* owner) : ICollisionComponent(owner)
{
	// オーナーがセットされていない場合は何もしない
	if(!owner)
	{
		return;
	}

	// OBBの初期化
	obb_.center = owner->GetPosition();
	obb_.rotate = MakeRotateMatrix(owner->GetRotation());
	obb_.size = owner->GetScale();
}

OBBColliderComponent::~OBBColliderComponent()
{

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
	// OBBを可視化する
	LineManager::GetInstance()->DrawOBB(obb_, VectorColorCodes::Cyan);

	if (useSweep_)
	{
		OBB previousObb = obb_;
		obb_.center = previousPosition_;
		obb_.rotate = MakeRotateMatrix(rotate);
		obb_.size = size;
		LineManager::GetInstance()->DrawOBB(obb_, VectorColorCodes::Red);
	}
}
