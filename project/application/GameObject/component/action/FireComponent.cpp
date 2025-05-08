#include "FireComponent.h"
#include <application/GameObject/base/GameObject.h>

#include "BulletComponent.h"
#include "3d/Object3dCommon.h"

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
    if (Input::GetInstance()->IsMouseButtonTriggered(0) && fireCooldownTimer_ <= 0.0f) {
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
    bullet->Initialize(object3dCommon_, lightManager_, owner->GetPosition());
    bullet->SetModel("cube.obj");

    // カメラ取得
    Camera* camera = object3dCommon_->GetDefaultCamera();
    if (!camera) return;

    // Inputからマウスのクライアント座標を取得
    float mouseX = Input::GetInstance()->GetMouseX();
    float mouseY = Input::GetInstance()->GetMouseY();

    // NDC（正規化デバイス座標）に変換
    float ndcX = (2.0f * mouseX) / WinApp::kClientWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / WinApp::kClientHeight;

    // NDC空間の近点と遠点（z = 0.0 と 1.0）
    Vector4 nearPointNDC = { ndcX, ndcY, 0.0f, 1.0f };
    Vector4 farPointNDC = { ndcX, ndcY, 1.0f, 1.0f };

    // ビュー・プロジェクションの逆行列
    Matrix4x4 view = camera->GetViewMatrix();
    Matrix4x4 proj = camera->GetProjectionMatrix();
    Matrix4x4 invViewProj = Inverse(view * proj);

    // NDC → ワールド空間
    Vector4 nearWorld = invViewProj * nearPointNDC;
    Vector4 farWorld = invViewProj * farPointNDC;
    nearWorld /= nearWorld.w;
    farWorld /= farWorld.w;

    // レイ計算（ワールド空間でマウス方向ベクトル）
    Vector3 rayOrigin = Vector3(nearWorld.x, nearWorld.y, nearWorld.z);
    Vector3 rayDir = Vector3::Normalize(Vector3(farWorld.x, farWorld.y, farWorld.z) - rayOrigin);

    // プレイヤーのY=0における地面とレイの交点を求める（t:rayの進み具合）
    Vector3 playerPos = owner->GetPosition();
    float t = (playerPos.y - rayOrigin.y) / rayDir.y;
    Vector3 hitPos = rayOrigin + rayDir * t;

    // 発射方向（水平面上）
    Vector3 direction = hitPos - playerPos;
    direction.y = 0.0f;
    direction = Vector3::Normalize(direction);

    // 弾の回転を設定（左手座標系では atan2f の引数順が右手座標系と同じ）
    float rotationY = atan2f(direction.x, direction.z);
    bullet->SetRotation({ 0.0f, rotationY, 0.0f });

    // BulletComponentを追加
    auto bulletComp = std::make_shared<BulletComponent>();
    bulletComp->Initialize(direction, 10.0f, 2.0f);
    bullet->AddComponent("Bullet", bulletComp);

    // 弾を管理リストに追加
    bullets_.push_back(bullet);
}

