#include "Bullet.h"

#include "application/GameObject/component/base/ICollisionComponent.h"

Bullet::~Bullet()
{
}

void Bullet::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, const Vector3& position)
{
	GameObject::Initialize(object3dCommon, lightManager); // GameObjectの初期化
	SetPosition(position); // 弾の初期位置を設定
	isAlive_ = true;
}

void Bullet::Update(float deltaTime)
{
	GameObject::Update(); // GameObjectの更新
}

void Bullet::Draw(CameraManager* camera)
{
	// 弾の描画処理（GameObjectの描画機能を使用）
	if (!isAlive_) return;
	GameObject::Draw(camera);
}

void Bullet::AddComponent(const std::string& name, std::shared_ptr<IGameObjectComponent> comp)
{
	if (auto collider = std::dynamic_pointer_cast<ICollisionComponent>(comp))
	{
		// 衝突判定コンポーネントの場合は、衝突時の処理を設定
		collider->SetOnEnter([this](GameObject* other) {
			// 衝突時の処理
			if (other->GetTag() == "enemy")
			{
				// 敵に当たった場合、弾を消す
				isAlive_ = false;
			}
							 });
	}
	GameObject::AddComponent(name, comp); // GameObjectにコンポーネントを追加
}
