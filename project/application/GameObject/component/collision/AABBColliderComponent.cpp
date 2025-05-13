#include "AABBColliderComponent.h"
#include "application/GameObject/base/GameObject.h"

AABBColliderComponent::AABBColliderComponent(GameObject* owner) : ICollisionComponent(owner), aabb_(Vector3(), Vector3())
{
}

void AABBColliderComponent::Update(GameObject* owner)
{
	// オーナーの位置とスケールを取得
	Vector3 pos = owner->GetPosition();
	Vector3 size = owner->GetScale();
	// AABBの更新
	aabb_.min_ = pos - size;
	aabb_.max_ = pos + size;
}
