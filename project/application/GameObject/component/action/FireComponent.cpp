#include "FireComponent.h"
#include <application/GameObject/base/GameObject.h>

#include "BulletComponent.h"

FireComponent::FireComponent(Object3dCommon* object3dCommon, LightManager* lightManager): fireCooldown_(0.5f), fireCooldownTimer_(0.0f)
{
	object3dCommon_ = object3dCommon;  // オブジェクトの共通情報
	lightManager_ = lightManager;  // ライトマネージャー
}

FireComponent::~FireComponent() {
    // デストラクタで発射された弾をすべて削除
    for (auto& bullet : bullets_) {
        bullet.reset();  // 弾のポインタをリセット
    }
    bullets_.clear();  // 弾リストをクリア
}

void FireComponent::Update(GameObject* owner) {
    fireCooldownTimer_ -= 1.0f / 60.0f;  // クールダウンタイマーを減らす

    // 発射ボタンが押され、クールダウンが終わったら弾を発射
    if (Input::GetInstance()->TriggerKey(DIK_SPACE) && fireCooldownTimer_ <= 0.0f) {
        FireBullet(owner);
        fireCooldownTimer_ = fireCooldown_; // クールダウンをリセット
    }

	// 弾の更新
	for (const auto& bullet : bullets_) {
		if (bullet->IsAlive()) {
			bullet->Update(1.0f / 60.0f);  // 生存している弾のみ更新
		}
	}

	//生存フラグが立っていない弾を削除
	for (auto it = bullets_.begin(); it != bullets_.end();) {
		if (!(*it)->IsAlive()) {
			it = bullets_.erase(it);  // 弾を削除
		}
		else {
			++it;  // 次の弾へ
		}
	}
}

void FireComponent::Draw(CameraManager* camera)
{
	// 弾の描画
	for (const auto& bullet : bullets_) {
		if (bullet->IsAlive()) {
			bullet->Draw(camera);  // 生存している弾のみ描画
		}
	}
}

void FireComponent::FireBullet(GameObject* owner) {
    // 弾の作成
    auto bullet = std::make_shared<Bullet>();
    bullet->Initialize(object3dCommon_,lightManager_,owner->GetPosition());  // 初期化
    bullet->SetModel("cube.obj");  // モデルの設定

    // 弾の移動方向（デフォルトで前方）
    Vector3 direction = { 0.0f, 0.0f, 1.0f };
    // プレイヤーの回転角度（Y軸回り）
    float rotationY = owner->GetRotation().y;

    // Y軸回りの回転を手動で行う
    direction.x = direction.x * cosf(rotationY) - direction.z * sinf(rotationY);
    direction.z = direction.x * sinf(rotationY) + direction.z * cosf(rotationY);
	bullet->SetRotation({ 0.0f, rotationY, 0.0f });  // 弾の回転を設定

    // BulletComponentを追加（弾の速度と寿命を設定）
    auto bulletComp = std::make_shared<BulletComponent>();
    bulletComp->Initialize(direction, 10.0f, 2.0f); // 速度と寿命

    // 弾にBulletComponentを追加
    bullet->AddComponent("Bullet", bulletComp);

    // 弾を管理リストに追加
    bullets_.push_back(bullet);  // ゲーム内に保持することも可能
}
