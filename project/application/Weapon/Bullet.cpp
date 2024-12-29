#include "Bullet.h"

void Bullet::Initialize(Object3dCommon* object3dCommon, const Vector3& position, const Vector3& direction)
{
	//生成した弾を初期化
	bullet_ = std::make_unique<Object3d>();
	bullet_->Initialize(object3dCommon);

	//引数で受けとった座標を記録
	bullet_->SetTranslate(position);

	//モデルをセットする
	bullet_->SetModel(filePath_);

	//移動方向を記録
	velocity_ = direction;
}

void Bullet::Update(CameraManager* camera)
{
	//生存時間のカウント
	elapsedTime_ += kDeltaTime;
	if (elapsedTime_ >= lifeTime_)
	{
		//寿命がすぎたのでフラグを下げる
		isAlive_ = false;
	}

	bullet_->SetTranslate(bullet_->GetTranslate() + velocity_);

	//行列の更新
	bullet_->Update(camera);
}

void Bullet::Draw()
{
	bullet_->Draw();
}
