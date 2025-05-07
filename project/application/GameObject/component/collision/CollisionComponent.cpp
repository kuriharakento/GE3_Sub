//#include "CollisionComponent.h"
//#include "application/GameObject/base/GameObject.h"
//
//void CollisionComponent::Update(GameObject* owner)
//{
//	UpdateColliderPosition(owner);
//}
//
//void CollisionComponent::UpdateColliderPosition(GameObject* owner)
//{
//	// コリジョンの位置を GameObject の位置に更新
//	if (collider_) {
//		collider_->SetPosition(owner->GetPosition());
//		collider_->SetScale(owner->GetScale());
//	}
//
//	// 衝突判定を行う
//	
//}
//
//void CollisionComponent::SetDefaultCallbacks()
//{
//	onEnter_ = [this](GameObject* other) {
//		// デフォルトの衝突開始処理
//		};
//	onStay_ = [this](GameObject* other) {
//		// デフォルトの衝突継続処理
//		};
//	onExit_ = [this](GameObject* other) {
//		// デフォルトの衝突終了処理
//		};
//}
