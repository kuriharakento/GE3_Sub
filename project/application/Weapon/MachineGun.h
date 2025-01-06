#pragma once
#include "IWeapon.h"
#include "Bullet.h"
#include <vector>
#include <numbers>

class Object3dCommon;
class Player;
class MachineGun : public IWeapon
{
public:
	// 初期化
    void Initialize(Object3dCommon* object3dCommon,Player* player);

    // 更新
    void Update(CameraManager* camera);

    // 弾丸の描画
    void Draw();

    // 武器を発射する
    void Shoot() override;

    // 現在の弾薬数を取得
    int GetCurrentAmmo() const override { return currentAmmo_; }

    // リロード中かどうか
    bool IsReloading() const override { return isReloading_; }

    // リロードを開始
    void StartReload() override;

    // リロード完了処理
    void FinishReload() override;

	// 弾のリストを取得
	std::vector<std::unique_ptr<Bullet>>& GetBullets() { return bullets_; }
	Bullet* GetBullet(int index) { return bullets_[index].get(); }

private:
	// 3Dオブジェクト共通データ
    Object3dCommon* object3dCommon_;

	// プレイヤー
	Player* player_;

	// 弾丸リスト
    std::vector<std::unique_ptr<Bullet>> bullets_;

	//1フレーム当たりの時間
	const float kDeltaTime = 1.0f / 60.0f;
	const float kDeg2Rad = std::numbers::pi_v<float> / 2.0f;

    // 弾薬数
    int currentAmmo_ = 30; // 初期弾薬数
    int magazineSize_ = 30; // マガジンサイズ
    int maxAmmo_ = 120; // 最大弾薬数

    // リロード関連
    bool isReloading_ = false;
    float reloadTime_ = 2.0f; // リロード時間
    float timeSinceLastReload_ = 0.0f;

    // 発射間隔
    float fireRate_ = 0.2f; // 連射速度
    float timeSinceLastShot_ = 0.0f;  
};
