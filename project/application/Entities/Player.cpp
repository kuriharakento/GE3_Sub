#include "Player.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif


#include "3d/ModelManager.h"
#include "input/Input.h"

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

	//カメラマネージャーのポインタ取得
	cameraManager_ = camera;

	//カメラマネージャーにプレイヤーを追尾するカメラを登録
	cameraManager_->AddCamera("FollowPlayer");
	//カメラマネージャーにプレイヤーを追尾するカメラをセット
	cameraManager_->SetActiveCamera("FollowPlayer");
}

void Player::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Player Status");
	ImGui::Text("Health : %.1f", status_.health);
	ImGui::Text("AttackPower : %.1f", status_.attackPower);
	ImGui::Text("Speed : %.3f", status_.speed);
	ImGui::DragFloat3("Position", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Rotation", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);
	ImGui::End();
#endif

	if(Input::GetInstance()->PushKey(DIK_W))
	{
		transform_.translate.z += status_.speed;
	}
	if (Input::GetInstance()->PushKey(DIK_S))
	{
		transform_.translate.z -= status_.speed;
	}
	if (Input::GetInstance()->PushKey(DIK_A))
	{
		transform_.translate.x -= status_.speed;
	}
	if (Input::GetInstance()->PushKey(DIK_D))
	{
		transform_.translate.x += status_.speed;
	}

	//カメラを追従させる
	Camera* camera = cameraManager_->GetCamera("FollowPlayer");
	camera->SetTranslate(transform_.translate + Vector3(0.0f, 1.0f, -3.0f));

	//行列の更新
	UpdateObjTransform();
}

void Player::Draw()
{
	object3d_->Draw();
}

void Player::OnCollision(ICollidable* other)
{
	other->GetAttackPower();
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
