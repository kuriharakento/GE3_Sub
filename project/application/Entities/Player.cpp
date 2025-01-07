#include "Player.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif


#include "3d/ModelManager.h"
#include "input/Input.h"
#include <algorithm>

#include "math/MathUtils.h"

void Player::Initialize(const std::string& filePath, Object3dCommon* objectCommon, SpriteCommon* spriteCommon, CameraManager* camera)
{
	// モデルの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(objectCommon);
	object3d_->SetModel(filePath);

	// Transformの初期化
	transform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,1.0f,0.0f}
	};

	transform_.scale = { 1.0f,1.0f,0.7f };

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

	//リロードUIの初期化
	reloadKeyUI_ = std::make_unique<Sprite>();
	reloadKeyUI_->Initialize(spriteCommon,"./Resources/reload.png");
	reloadKeyUI_->SetAnchorPoint(Vector2(0.5f, 0.5f));
	reloadKeyUI_->SetPosition(Vector2(640.0f,550.0f));

	//リロード中UIの初期化
	reloadingUI_ = std::make_unique<Sprite>();
	reloadingUI_->Initialize(spriteCommon, "./Resources/reloading.png");
	reloadingUI_->SetAnchorPoint(Vector2(0.5f, 0.5f));
	reloadingUI_->SetPosition(Vector2(640.0f, 100.0));

	//HpUI
	hpUI_ = std::make_unique<Sprite>();
	hpUI_->Initialize(spriteCommon, "./Resources/HP.png");
	hpUI_->SetAnchorPoint(Vector2(0.5f, 0.5f));
	hpUI_->SetPosition(Vector2(640.0f, 650.0f));
}

void Player::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Player Status");
	//スプライト
	Vector2 spritePosition = reloadKeyUI_->GetPosition();
	ImGui::DragFloat2("SpritePosition", &spritePosition.x, 0.1f);
	reloadKeyUI_->SetPosition(spritePosition);
	Vector2 reloadingPosition = reloadingUI_->GetPosition();
	ImGui::DragFloat2("ReloadingPosition", &reloadingPosition.x, 0.1f);
	reloadingUI_->SetPosition(reloadingPosition);
	ImGui::DragFloat("CameraZOffset", &cameraZOffset_, 0.1f);
	ImGui::Text("Health : %.1f", status_.health);
	ImGui::Text("AttackPower : %.1f", status_.attackPower);
	ImGui::Text("Speed : %.3f", status_.speed);
	ImGui::DragFloat3("Position", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Rotation", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);
	ImGui::End();
#endif

	if(status_.health <= 0.0f)
	{
		status_.isAlive = false;
	}

	//移動
	Move();

	//カメラの更新
	CameraUpdate();

	//武器の更新
	machineGun_->Update(cameraManager_);

	//スプライトの更新
	reloadKeyUI_->Update();
	reloadingUI_->Update();
	hpUI_->SetSize(Vector2(status_.health * 3.0f, 25.0f));
	hpUI_->Update();

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

void Player::DrawUI()
{
	if (machineGun_->IsReloading())
	{
		reloadingUI_->Draw();
	}
	if (machineGun_->GetCurrentAmmo() <= 0 && !machineGun_->IsReloading())
	{
		reloadKeyUI_->Draw();
	}
	if (status_.health > 0.0f)
	{
		hpUI_->Draw();
	}
}

void Player::OnCollision(ICollidable* other)
{
	if (other->GetType() == ObjectType::Bullet) { return; }
	Camera* camera = cameraManager_->GetCamera(followCameraName_);
	camera->StartShake(1.5f, 0.5f);

	status_.health -= other->GetAttackPower();
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

	//移動範囲を制限
	Vector2 moveRange = { 100.0f,100.0f };
	if (transform_.translate.x < -moveRange.x)
	{
		transform_.translate.x = -moveRange.x;
	}
	if (transform_.translate.x > moveRange.x)
	{
		transform_.translate.x = moveRange.x;
	}

	if (transform_.translate.z < -moveRange.y)
	{
		transform_.translate.z = -moveRange.y;
	}
	if (transform_.translate.z > moveRange.y)
	{
		transform_.translate.z = moveRange.y;
	}
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
    camera->SetRotate(Vector3(-0.06f, yaw, 0.0f));
	transform_.rotate = camera->GetRotate();
}
