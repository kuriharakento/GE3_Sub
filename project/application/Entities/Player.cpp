#include "Player.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif


#include "3d/ModelManager.h"
#include "input/Input.h"
#include <algorithm>

#include "math/MathUtils.h"

void Player::Initialize(const std::string& filePath,Object3dCommon* objectCommon, CameraManager* camera)
{
	// モデルの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(objectCommon);
	object3d_->SetModel(filePath);

	// Transformの初期化
	transform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	//スケールと当たり判定を一致させる
	hitBox_.min = -transform_.scale;
	hitBox_.max = transform_.scale;

	//武器の初期化
	machineGun_ = std::make_unique<MachineGun>();
	machineGun_->Initialize(objectCommon,this);

	//カメラマネージャーのポインタ取得
	cameraManager_ = camera;

	//カメラマネージャーにプレイヤーを追尾するカメラを登録
	cameraManager_->AddCamera(followCameraName_);
	//カメラマネージャーにプレイヤーを追尾するカメラをセット
	cameraManager_->SetActiveCamera(followCameraName_);
}

void Player::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Player Status");
	ImGui::DragFloat("CameraZOffset", &cameraZOffset_, 0.1f);
	ImGui::Text("Health : %.1f", status_.health);
	ImGui::Text("AttackPower : %.1f", status_.attackPower);
	ImGui::Text("Speed : %.3f", status_.speed);
	ImGui::DragFloat3("Position", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Rotation", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);
	ImGui::End();
#endif

	//移動
	Move();

	//カメラの更新
	CameraUpdate();

	//武器の更新
	machineGun_->Update(cameraManager_);

	//行列の更新
	UpdateObjTransform();
}

void Player::Draw()
{
	//プレイヤーの描画
	object3d_->Draw();

	//武器の描画
	machineGun_->Draw();
}

void Player::OnCollision(ICollidable* other)
{
	if (other->GetType() == ObjectType::Bullet) { return; }
	Camera* camera = cameraManager_->GetCamera(followCameraName_);
	camera->StartShake(0.5f, 0.3f);
}

ObjectType Player::GetType() const
{
	return type_;
}

float Player::GetHealth() const
{
	return status_.health;
}

float Player::GetAttackPower() const
{
	return status_.attackPower;
}

float Player::GetSpeed() const
{
	return status_.speed;
}

const AABB& Player::GetBoundingBox() const
{
	return hitBox_;
}

void Player::Move()
{
	Vector3 velocity = { 0.0f,0.0f,0.0f };
	if (Input::GetInstance()->PushKey(DIK_W))
	{
		velocity.z += status_.speed;
	}
	if (Input::GetInstance()->PushKey(DIK_S))
	{
		velocity.z -= status_.speed;
	}
	if (Input::GetInstance()->PushKey(DIK_A))
	{
		velocity.x -= status_.speed;
	}
	if (Input::GetInstance()->PushKey(DIK_D))
	{
		velocity.x += status_.speed;
	}

	if (velocity.Length() > 0.0f)
	{
		velocity = velocity.Normalize() * status_.speed;
	}

	velocity = MathUtils::Transform(velocity, MakeRotateYMatrix(cameraManager_->GetCamera(followCameraName_)->GetRotate().y));
	transform_.translate += velocity;
}


void Player::CameraUpdate()
{
	// カメラを取得
    Camera* camera = cameraManager_->GetCamera(followCameraName_);

    // マウスの移動量を取得
    float mouseX = static_cast<float>(Input::GetInstance()->GetMouseDeltaX());
    float mouseY = static_cast<float>(Input::GetInstance()->GetMouseDeltaY());

    // マウスの入力に基づいてカメラの回転角度を更新
    cameraYaw_ += mouseX * 0.003f;  // 感度調整
    cameraPitch_ += mouseY * 0.003f;  // 感度調整

    // ピッチ角度を制限
    if (cameraPitch_ > DirectX::XM_PIDIV4) cameraPitch_ = DirectX::XM_PIDIV4;  // 45度に制限
    if (cameraPitch_ < -DirectX::XM_PIDIV4) cameraPitch_ = -DirectX::XM_PIDIV4;  // -45度に制限

	// カメラの位置を計算
	Vector3 offset;
	offset.x = cameraZOffset_ * sinf(cameraYaw_);
	offset.y = 1.5f;  // プレイヤーの上に固定オフセット
	offset.z = cameraZOffset_ * cosf(cameraYaw_);

    // カメラの位置を更新
    camera->SetTranslate(transform_.translate + offset);

    // カメラの回転を設定
    Vector3 direction = transform_.translate - camera->GetTranslate();
    direction.Normalize();
    float pitch = asinf(direction.y);
    float yaw = atan2f(direction.x, direction.z);
    camera->SetRotate(Vector3(0.0f, yaw, 0.0f));
	transform_.rotate = Vector3(0.0f, yaw, 0.0f);
}
