#include "Missile.h"
#include "math/VectorFunc.h"

Missile::Missile()
{
	status_.isAlive = true;
	status_.attackPower = 10.0f;
	status_.speed = 2.0f;
	status_.health = 30.0f;
}

void Missile::Initialize(Object3dCommon* object3dCommon, const Vector3& startPosition, const Vector3& controlPoint, const Vector3& endPosition)
{
    // ミサイルモデルを生成
    missile_ = std::make_unique<Object3d>();
    missile_->Initialize(object3dCommon);
	ModelManager::GetInstance()->LoadModel(filePath_);
	missile_->SetModel(filePath_);

	// ベジェ曲線の制御点を設定
    startPoint_ = startPosition;
    controlPoint_ = controlPoint;
    endPoint_ = endPosition;

	// ミサイルの初期位置を設定
    missile_->SetTranslate(startPosition);
	missile_->SetScale({ 0.7f,0.7f,0.7f });

	//ミサイルの当たり判定タイプを設定
	type_ = ObjectType::Missile;

	// ミサイルの当たり判定用のAABBを設定
	hitBox_.min = -Vector3(1.0f,1.0f,1.0f);
	hitBox_.max = Vector3(1.0f, 1.0f, 1.0f);
}

void Missile::Update(CameraManager* camera)
{
    if (!status_.isAlive) return;

    // 時間を更新（スピード倍率を掛ける）
    elapsedTime_ += kDeltaTime * status_.speed;
    float t = elapsedTime_ / lifeTime_;

    // 寿命を超えたら破棄
    if (t >= 1.0f)
    {
        status_.isAlive = false;
        return;
    }

    Vector3 position;
    position.x =
        (1 - t) * (1 - t) * startPoint_.x +
        2 * (1 - t) * t * controlPoint_.x +
        t * t * endPoint_.x;

    position.y =
        (1 - t) * (1 - t) * startPoint_.y +
        2 * (1 - t) * t * controlPoint_.y +
        t * t * endPoint_.y;

    position.z =
        (1 - t) * (1 - t) * startPoint_.z +
        2 * (1 - t) * t * controlPoint_.z +
        t * t * endPoint_.z;


    missile_->SetTranslate(position);

	missile_->Update(camera);
}

void Missile::Draw()
{
    if (!status_.isAlive) return;
    missile_->Draw();
}

void Missile::OnCollision(ICollidable* other)
{
    if(other->GetType() == ObjectType::Player)
    {
		status_.isAlive = false;
    }
}

