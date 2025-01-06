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

	//ヒットボックスの初期化
    hitBox_.min = -transform_.scale;
	hitBox_.max = transform_.scale;
}

void Enemy::Update(CameraManager* camera)
{
#ifdef _DEBUG
    ImGui::Begin("Enemy");
    float distance = (player_->GetPosition() - transform_.translate).Length();
    ImGui::Text("range %f",distance);
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

    // ステートごとの動作を実行
    switch (state_)
    {
    case State::Idle:
        UpdateIdle();
        break;
    case State::Chase:
        UpdatePatrol();
        UpdateAttack();
        break;
    case State::Approach:
		UpdateApproach();
        break;
    }

    // 発射タイマーを更新
    fireTimer_ -= 1.0f / 60.0f;

	//行列の更新
	UpdateObjTransform(camera);
}

void Enemy::Draw()
{
    if (status_.isAlive) 
    {
        object3d_->Draw();
    }

	// ミサイルの描画
	for (auto& missile : missiles_)
    {
		missile->Draw();
	}
}

void Enemy::OnCollision(ICollidable* other)
{
	// 衝突時の処理を記述
	if (other->GetType() == ObjectType::Bullet)
	{
		status_.isAlive = false;
	}
}

void Enemy::SetPosition(const Vector3& position)
{
	transform_.translate = position;
}

// 待機状態の更新
void Enemy::UpdateIdle()
{
    // プレイヤーが一定範囲に入ったら攻撃モードに遷移
    if (IsPlayerInRange(patrolRange_))
    {
        state_ = State::Chase;
    }
}

// 巡回状態の更新
void Enemy::UpdatePatrol()
{
    // ランダムに方向転換するためのタイマー
    const float directionChangeInterval = 3.0f; // 方向転換の間隔（秒）
	const float deltaTime = 1.0f / 60.0f; // 1フレームあたりの時間（秒）

    // 一定の間隔ごとにランダムな方向に進む
    if (patrolTimer_ >= directionChangeInterval)
    {
        // ランダムな方向を設定
        float randomX = (std::rand() % 2 == 0 ? 1.0f : -1.0f); // 1か-1のランダム
        float randomZ = (std::rand() % 2 == 0 ? 1.0f : -1.0f); // 1か-1のランダム

		//ランダムなスピードを設定
        float randomSpeed = 0.01f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (0.04 - 0.02f)));

        // 新しい方向の設定
        patrolDirection_ = Vector3(randomX, 0.0f, randomZ); // Y軸方向は固定

		// スピードの設定
		status_.speed = randomSpeed;

        // タイマーリセット
        patrolTimer_ = 0.0f;
    }

    // 巡回方向に移動
    transform_.translate += patrolDirection_ * status_.speed;

    // 時間経過を反映
    patrolTimer_ += deltaTime;  // deltaTimeを加算してタイマーを進める

	// プレイヤーが射程外に出たら待機状態に戻る
    if (!IsPlayerInRange(patrolRange_))
    {
		state_ = State::Approach;
    }
}

// 攻撃状態の更新
void Enemy::UpdateAttack()
{
    fireTimer_ -= 1.0f / 60.0f; // 攻撃タイマーを減少

    // プレイヤーが射程外に出た場合、待機状態に戻る
    if (!IsPlayerInRange(15.0f))
    {
        state_ = State::Approach;
        return;
    }

    // ミサイル発射
    if (fireTimer_ <= 0.0f)
    {
        FireMissile();
        fireTimer_ = fireInterval_; // タイマーリセット
    }
}

//
void Enemy::UpdateApproach()
{
    // プレイヤーに近づく
    Vector3 direction = player_->GetPosition() - transform_.translate;
    direction.Normalize();
    transform_.translate += direction * (status_.speed / 10);

    // 一定距離近づいたら待機状態に戻る
    if (IsPlayerInRange(retreatRange_))
    {
        state_ = State::Idle;
    }

}

// プレイヤーとの距離チェック
bool Enemy::IsPlayerInRange(float range)
{
    float distance = (player_->GetPosition() - transform_.translate).Length();
    return distance <= range;
}

// ミサイル発射
void Enemy::FireMissile()
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