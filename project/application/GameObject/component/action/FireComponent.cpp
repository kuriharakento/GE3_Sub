#include "FireComponent.h"
#include <application/GameObject/base/GameObject.h>

#include "BulletComponent.h"
#include "3d/Object3dCommon.h"
#include "application/GameObject/component/collision/OBBColliderComponent.h"
#include "base/Logger.h"

FireComponent::FireComponent(Object3dCommon* object3dCommon, LightManager* lightManager) : fireCooldown_(0.5f), fireCooldownTimer_(0.0f)
{
	object3dCommon_ = object3dCommon;  // オブジェクトの共通情報
	lightManager_ = lightManager;  // ライトマネージャー
}

FireComponent::~FireComponent()
{
	// デストラクタで発射された弾をすべて削除
	for (auto& bullet : bullets_)
	{
		bullet.reset();  // 弾のポインタをリセット
	}
	bullets_.clear();  // 弾リストをクリア
}

void FireComponent::Update(GameObject* owner)
{
	fireCooldownTimer_ -= 1.0f / 60.0f;  // クールダウンタイマーを減らす

	// 発射ボタンが押され、クールダウンが終わったら弾を発射
	if (Input::GetInstance()->IsMouseButtonTriggered(0) && fireCooldownTimer_ <= 0.0f)
	{
		FireBullet(owner);
		fireCooldownTimer_ = fireCooldown_; // クールダウンをリセット
	}

	// 弾の更新
	for (const auto& bullet : bullets_)
	{
		if (bullet->IsAlive())
		{
			bullet->Update(1.0f / 60.0f);  // 生存している弾のみ更新
		}
	}

	//生存フラグが立っていない弾を削除
	for (auto it = bullets_.begin(); it != bullets_.end();)
	{
		if (!(*it)->IsAlive())
		{
			it = bullets_.erase(it);  // 弾を削除
		}
		else
		{
			++it;  // 次の弾へ
		}
	}
}

void FireComponent::Draw(CameraManager* camera)
{
	// 弾の描画
	for (const auto& bullet : bullets_)
	{
		if (bullet->IsAlive())
		{
			bullet->Draw(camera);  // 生存している弾のみ描画
		}
	}
}

void FireComponent::FireBullet(GameObject* owner)
{
	// 弾の作成
	auto bullet = std::make_shared<Bullet>("Bullet");

	// カメラ取得
	Camera* camera = object3dCommon_->GetDefaultCamera();
	if (!camera) return;

	// マウスのスクリーン座標を取得
	float mouseX = Input::GetInstance()->GetMouseX();
	float mouseY = Input::GetInstance()->GetMouseY();

	// ビューポート行列を作成
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);

	// ビュー行列とプロジェクション行列を取得し、ビューポート行列と合成
	Matrix4x4 matVPV = (camera->GetViewMatrix() * camera->GetProjectionMatrix()) * matViewport;

	// 合成行列の逆行列を計算
	Matrix4x4 matInverseVPV = Inverse(matVPV);

	// スクリーン座標を定義（近点と遠点）
	Vector3 posNear = Vector3(mouseX, mouseY, 0.0f); // 近クリップ面
	Vector3 posFar = Vector3(mouseX, mouseY, 1.0f);  // 遠クリップ面

	// スクリーン座標をワールド座標に変換
	posNear = MathUtils::Transform(posNear, matInverseVPV);
	posFar = MathUtils::Transform(posFar, matInverseVPV);

	// レイの方向を計算
	Vector3 rayDir = Vector3::Normalize(posFar - posNear);

	// プレイヤーの位置を取得
	Vector3 playerPos = owner->GetPosition();

	// 地面（Y=0）との交点を計算
	float t = -posNear.y / rayDir.y; // Y=0 の平面との交点
	Vector3 targetPos = posNear + rayDir * t;

	// 発射方向を計算
	Vector3 direction = Vector3::Normalize(targetPos - playerPos);
	direction.y = 0.0f; // Y成分を0にすることで水平方向のベクトルにする

	// 水平方向の角度を計算
	float rotationY = atan2f(direction.x, direction.z);

	// 弾の初期化
	bullet->Initialize(object3dCommon_, lightManager_, playerPos);
	bullet->SetModel("cube.obj");
	bullet->SetRotation({ 0.0f, rotationY, 0.0f });

	// BulletComponentを追加
	auto bulletComp = std::make_shared<BulletComponent>();
	bulletComp->Initialize(direction, 30.0f, 2.0f); // 速度: 10.0f, 寿命: 2.0f
	bullet->AddComponent("Bullet", bulletComp);

	// 衝突判定コンポーネントを追加
	auto colliderComp = std::make_shared<OBBColliderComponent>(bullet.get());
	colliderComp->SetOnEnter([bullet](GameObject* other) {
		if (other->GetTag() == "enemy")
		{
			// 敵に当たった場合の処理
			bullet->SetActive(false);  // 弾を非アクティブにする
		}
							 });
	bullet->AddComponent("OBBCollider", colliderComp);

	// 弾を管理リストに追加
	bullets_.push_back(bullet);
}
