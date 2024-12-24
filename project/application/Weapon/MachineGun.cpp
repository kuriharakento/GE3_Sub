#include "MachineGun.h"

#include "input/Input.h"
#include "application/Entities/Player.h"
#include "externals/imgui/imgui.h"

void MachineGun::Initialize(Object3dCommon* object3dCommon, Player* player)
{
	// 3Dオブジェクト共通データを記憶
	object3dCommon_ = object3dCommon;

	// 弾リストをクリア
	bullets_.clear();

	// プレイヤーを記憶
	player_ = player;
}

void MachineGun::Update(CameraManager* camera)
{
#ifdef _DEBUG
	ImGui::Begin("MachineGun");
	ImGui::Text("Bullets %d", bullets_.size());
    if(ImGui::CollapsingHeader("Bullets"))
    {
		for (auto& bullet : bullets_)
		{
			// 位置を変更
			Vector3 position = bullet->GetPosition();
			ImGui::DragFloat3("Position", &position.x, 0.1f);
			bullet->SetPosition(position);
			// スケールを変更
			Vector3 scale = bullet->GetScale();
			ImGui::DragFloat3("Scale", &scale.x, 0.1f);
			bullet->SetScale(scale);
			// 回転を変更
			Vector3 rotate = bullet->GetRotation();
			ImGui::DragFloat3("Rotate", &rotate.x, 0.1f);
			bullet->SetRotation(rotate);
		}
    }
    ImGui::End();
#endif


    if (isReloading_)
    {
        // リロード中ならリロード時間を進める
        timeSinceLastReload_ += kDeltaTime;
        if (timeSinceLastReload_ >= reloadTime_)
        {
            FinishReload(); // リロード完了
        }
    }

    //リロード
	if (Input::GetInstance()->PushKey(DIK_R))
    {
		StartReload();
	}

    // 発射間隔の管理
    timeSinceLastShot_ += kDeltaTime;
    if (timeSinceLastShot_ >= fireRate_ && currentAmmo_ > 0 && !isReloading_)
    {
        if(Input::GetInstance()->PushKey(DIK_SPACE))
        {
        	Shoot(player_->GetPosition(), Vector3(0.0f, 0.0f, 0.2f));
        }
    }

    // 弾丸の更新
    for (auto& bullet : bullets_)
    {
        bullet->Update(camera);
    }

	// 弾丸の削除
	for (auto it = bullets_.begin(); it != bullets_.end();)
	{
		if (!(*it)->IsAlive())
		{
			it = bullets_.erase(it);
		} else
		{
			++it;
		}
	}

}

void MachineGun::Draw()
{
	for (auto& bullet : bullets_)
	{
		bullet->Draw();
	}
}

void MachineGun::Shoot(const Vector3& position, const Vector3& direction)
{
    if (isReloading_ || currentAmmo_ <= 0)
        return; // リロード中または弾薬がない場合は発射しない

    // 弾丸を発射
    std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();
    newBullet->Initialize(object3dCommon_, position, direction);
    bullets_.push_back(std::move(newBullet));  // 弾丸をリストに追加

    // 弾薬を減らす
    --currentAmmo_;

    // 発射後の時間をリセット
    timeSinceLastShot_ = 0.0f;
}

void MachineGun::StartReload()
{
	//既にリロード中または弾薬が満タンの場合はリロードしない
    if (isReloading_ || currentAmmo_ == magazineSize_) {
        return;
    }

    // リロード開始
    isReloading_ = true;
    timeSinceLastReload_ = 0.0f;
}

void MachineGun::FinishReload()
{
	// リロード中でなければ処理しない
    if (!isReloading_) {
        return;
    }

    // 弾薬を満タンにする
    currentAmmo_ = magazineSize_;
    isReloading_ = false;
}


