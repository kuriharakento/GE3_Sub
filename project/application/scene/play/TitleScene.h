#pragma once
#include <memory>

#include "application/GameObject/character/enemy/PistolEnemy.h"
#include "2d/Sprite.h"
#include "3d/Object3d.h"
#include "application/Animation/Slide.h"
#include "application/GameObject/base/GameObject.h"
#include "application/GameObject/character/enemy/EnemyManager.h"
#include "application/GameObject/character/player/Player.h"
#include "application/GameObject/obstacle/ObstacleManager.h"
#include "camerawork/FollowCamera.h"
#include "camerawork/OrbitCameraWork.h"
#include "camerawork/SplineCamera.h"
#include "camerawork/TopDownCamera.h"
#include "effects/ParticleEmitter.h"
#include "engine/scene/interface/BaseScene.h"

class TitleScene : public BaseScene
{
public:
	//初期化
	void Initialize() override;
	//終了
	void Finalize() override;
	//更新
	void Update() override;
	//描画
	void Draw3D() override;
	void Draw2D() override;

private:
	// パーティクルエミッターの初期化
	void InitializeParticleEmitters();

private: //メンバ変数
	//スカイドーム
	std::unique_ptr<Object3d> skydome_;
	//地面
	std::unique_ptr<Object3d> ground_;
	//カメラワーク
	std::unique_ptr<SplineCamera> splineCamera_;
	std::unique_ptr<TopDownCamera> topDownCamera_;
	//ゲームオブジェクト
	std::unique_ptr<Player> player;
	std::unique_ptr<EnemyManager> enemyManager_;
	std::unique_ptr<ObstacleManager> obstacleManager_;
	//エミッター
	std::unique_ptr<ParticleEmitter> dust_;
	std::unique_ptr<ParticleEmitter> redEffect_;
	std::unique_ptr<ParticleEmitter> fallHeart_;
	std::unique_ptr<ParticleEmitter> glitch_;
	std::unique_ptr<ParticleEmitter> mordeVFXGround_;
	std::unique_ptr<ParticleEmitter> mordeVFXFragment_;
};
