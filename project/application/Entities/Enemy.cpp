#include "Enemy.h"
#include "application/Entities/Player.h"
#include <cstdlib>
#include <ctime>

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

void Enemy::Initialize(const std::string& filePath, Object3dCommon* objectCommon)
{
	//オブジェクトの初期化
	objectCommon_ = objectCommon;
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(objectCommon_);
	object3d_->SetModel(filePath);

	//座標の初期化
	transform_ = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,10.0f }
	};
}

void Enemy::Update(CameraManager* camera)
{
#ifdef _DEBUG
    ImGui::Begin("Enemy");
    if(ImGui::CollapsingHeader("missile"))
    {
		for (auto& missile : missiles_)
	    {
			//位置を変更
		    Vector3 position = missile->GetPosition();
		    ImGui::DragFloat3("Position", &position.x, 0.1f);
		    missile->SetPosition(position);
		    //スケールを変更
		    Vector3 scale = missile->GetScale();
		    ImGui::DragFloat3("Scale", &scale.x, 0.1f);
		    missile->SetScale(scale);
		    //回転を変更
		    Vector3 rotate = missile->GetRotation();
		    ImGui::DragFloat3("Rotate", &rotate.x, 0.1f);
		    missile->SetRotation(rotate);
	    }
    }
	ImGui::End();
#endif

    UpdateMissile();

	//行列の更新
	UpdateObjTransform(camera);
}

void Enemy::Draw()
{
	object3d_->Draw();

	// ミサイルの描画
	for (auto& missile : missiles_)
    {
		missile->Draw();
	}
}

void Enemy::OnCollision(ICollidable* other)
{
	// 衝突時の処理を記述
	other->GetAttackPower();
}

void Enemy::UpdateMissile()
{
    // 発射タイマーを更新
    fireTimer_ -= 1.0f / 60.0f;

    // 発射可能な場合
    if (fireTimer_ <= 0.0f)
    {
		//ベジェ曲線の制御点
        Vector3 startPosition = transform_.translate;
        Vector3 endPosition = player_->GetPosition();
		// 制御点の高さとランダムオフセットを計算
		float randomOffsetY = 20.0f;
		float randomOffsetX = static_cast<float>(std::rand() % 10 - 5);
		float randomOffsetZ = static_cast<float>(std::rand() % 10 - 5);
		// 制御点を計算
		Vector3 controlPoint =
			(startPosition + endPosition) * 0.5f +
			Vector3(randomOffsetX, randomOffsetY, randomOffsetZ);

        // ミサイルを生成
        auto missile = std::make_unique<Missile>();
        missile->Initialize(objectCommon_, startPosition, controlPoint, endPosition);

        // リストに追加
        missiles_.emplace_back(std::move(missile));

        // タイマーをリセット
        fireTimer_ = fireInterval_;
    }


    // 無効なミサイルを削除
    missiles_.erase(
        std::remove_if(missiles_.begin(), missiles_.end(), [](const std::unique_ptr<Missile>& missile) {
            return !missile->IsAlive();
            }),
        missiles_.end());
}
