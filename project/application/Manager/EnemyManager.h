#pragma once
#include <vector>
#include <memory>
#include <string>

#include "3d/Object3dCommon.h"
#include "application/Entities/Enemy.h"
#include "manager/CameraManager.h"

class Player;
class EnemyManager
{
public:
    // 初期化
    void Initialize(Object3dCommon* objectCommon, CameraManager* camera, Player* player, const std::string& filePath);

    // 更新
    void Update();

    // 描画
    void Draw();

    // 敵を指定数生成
    void SpawnEnemies(int count, const Vector3& basePosition, const Vector3& offset = Vector3());

    // 敵のリストを取得
    const std::vector<std::unique_ptr<Enemy>>& GetEnemies() const { return enemies_; }
	Enemy* GetEnemy(int index) { return enemies_[index].get(); }

private:
    // 敵のリスト
    std::vector<std::unique_ptr<Enemy>> enemies_;

    // 共通オブジェクト
    Object3dCommon* objectCommon_ = nullptr;

	// カメラマネージャー
	CameraManager* cameraManager_ = nullptr;

    //プレイヤー
	Player* player_ = nullptr;

    // 敵モデルのファイルパス
    std::string filePath_;
};