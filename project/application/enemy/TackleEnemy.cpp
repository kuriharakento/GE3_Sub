#include "TackleEnemy.h"

#include "math/Easing.h"

void TackleEnemy::Initialize(Object3dCommon* object3dCommon, const std::string& filePath)
{
	//オブジェクトの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(object3dCommon);
	object3d_->SetModel(filePath);
	//トランスフォームの初期化
	transform_ = {
		{ 1.0,1.0,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f }
	};
}

void TackleEnemy::Update(CameraManager* camera)
{
	// タックルの更新
	UpdateTackle();

	//オブジェクトの更新
	object3d_->SetTranslate(transform_.translate);
	object3d_->SetRotate(transform_.rotate);
	object3d_->SetScale(transform_.scale);
	object3d_->Update(camera);
}

void TackleEnemy::Draw()
{
	object3d_->Draw();
}

void TackleEnemy::StartTackle(const Vector3& targetPosition)
{
	if (!isTackling_) {
		isTackling_ = true;

		// ターゲット方向を計算
		tackleDirection_ = targetPosition - transform_.translate;
		// Y軸方向の移動はしない
		tackleDirection_.y = 0.0f;
		// 正規化
		tackleDirection_ = Vector3::Normalize(tackleDirection_);

		// 初期速度を設定
		tackleVelocity_ = tackleDirection_ * tackleSpeed_;
	}
}

void TackleEnemy::UpdateTackle()
{
	if (isTackling_) {
		// フレーム間の時間差（秒）
		float deltaTime = 1.0f / 60.0f;

		// 摩擦による減速を適用
		Vector3 friction = -tackleVelocity_ * tackleFriction_ * deltaTime;
		tackleVelocity_ += friction;

		// 速度が非常に小さくなったら停止する
		if (Vector3::Length(tackleVelocity_) < 0.01f) {
			tackleVelocity_ = { 0.0f, 0.0f, 0.0f };
			isTackling_ = false;
			return;
		}

		// 位置を更新
		transform_.translate += tackleVelocity_ * deltaTime;
	}
}



