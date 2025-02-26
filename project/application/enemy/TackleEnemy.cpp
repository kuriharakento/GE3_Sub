#include "TackleEnemy.h"

#include "imgui/imgui.h"
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
	isTackling_ = false;
	tackleSpeed_ = 8.0f; // 初期速度
	tackleAcceleration_ = -10.0f; // 加速度（負の値で減速）
	tackleDuration_ = 1.0f; // タックルの持続時間
	tackleTimer_ = 0.0f;
	target_ = { 0.0f,0.0f,3.0f };
}

void TackleEnemy::Update(CameraManager* camera)
{
#ifdef _DEBUG
	ImGui::Begin("TackleEnemy");
	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);
	if (ImGui::Button("Tackle")) {
		StartTackle(target_);
	}
	ImGui::DragFloat3("Targetposition", &target_.x, 0.01f);
	ImGui::End();
#endif

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
		tackleTimer_ = 0.0f;
		// ターゲット方向を計算
		tackleDirection_ = targetPosition - transform_.translate;
		tackleDirection_ = Vector3::Normalize(tackleDirection_);
	}
}

void TackleEnemy::UpdateTackle()
{
	if (isTackling_) {
		tackleTimer_ += 1.0f / 60.0f;
		if (tackleTimer_ < tackleDuration_) {
			// 現在の速度を計算
			float currentSpeed = tackleSpeed_ + tackleAcceleration_ * tackleTimer_;
			// 速度が負にならないようにする
			if (currentSpeed < 0.0f) {
				currentSpeed = 0.0f;
			}
			Vector3 movement = tackleDirection_ * currentSpeed * (1.0f / 60.0f);
			transform_.translate.x += movement.x;
			transform_.translate.y += movement.y;
			transform_.translate.z += movement.z;
		}
		else {
			isTackling_ = false;
			tackleTimer_ = 0.0f;
		}
	}
}



