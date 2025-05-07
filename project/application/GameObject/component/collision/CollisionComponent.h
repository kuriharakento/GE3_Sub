//#pragma once
//#include <functional>
//#include <memory>
//
//#include "application/GameObject/base/IGameObjectComponent.h"
//#include "application/GameObject/collider/ICollider.h"
//
//class GameObject;
//
//class CollisionComponent : public IGameObjectComponent {
//public:
//    using CollisionCallback = std::function<void(GameObject*)>;
//
//    // コンストラクタで GameObject と ICollider を受け取る
//    CollisionComponent(GameObject* owner, std::shared_ptr<ICollider> collider)
//        : owner_(owner), collider_(std::move(collider)) {}
//
//    // デストラクタ
//    ~CollisionComponent() = default;
//
//    // IGameObjectComponent の Update を実装
//    void Update(GameObject* owner) override;
//
//    // Collider 更新のための関数
//    void UpdateColliderPosition(GameObject* owner);
//
//    // コールバック設定
//    void SetOnCollisionEnter(CollisionCallback callback) { onEnter_ = std::move(callback); }
//    void SetOnCollisionStay(CollisionCallback callback) { onStay_ = std::move(callback); }
//    void SetOnCollisionExit(CollisionCallback callback) { onExit_ = std::move(callback); }
//
//    // コールバックを呼ぶ
//    void OnCollisionEnter(GameObject* other) { if (onEnter_) onEnter_(other); }
//    void OnCollisionStay(GameObject* other) { if (onStay_)  onStay_(other); }
//    void OnCollisionExit(GameObject* other) { if (onExit_)  onExit_(other); }
//
//    // コリジョン情報取得
//    std::shared_ptr<ICollider> GetCollider() const { return collider_; }
//
//    // GameObject を取得
//    GameObject* GetOwner() const { return owner_; }
//
//    // 自動的にコールバックを設定する便宜的なメソッドを追加
//    void SetDefaultCallbacks();
//
//	ICollider* GetCollider() { return collider_.get(); }
//
//private:
//    GameObject* owner_;  // 所有している GameObject
//    std::shared_ptr<ICollider> collider_;  // その GameObject の Collider
//
//    // 衝突判定のコールバック関数
//    CollisionCallback onEnter_;
//    CollisionCallback onStay_;
//    CollisionCallback onExit_;
//};
