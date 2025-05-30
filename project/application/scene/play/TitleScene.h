#pragma once
#include <memory>

#include "2d/Sprite.h"
#include "3d/Object3d.h"
#include "application/Animation/Slide.h"
#include "application/GameObject/base/GameObject.h"
#include "application/GameObject/character/player/Player.h"
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

private: //メンバ変数
	//デバック用オブジェクト
	std::unique_ptr<Object3d> object3d_;
	//デバック用オブジェクト地面
	std::unique_ptr<Object3d> terrain_;
	//スカイドーム
	std::unique_ptr<Object3d> skydome_;
	//キューブの座標
	Vector3 cubePos1_ = { 0.0f,0.0f,0.0f };
	Vector3 cubePos2_ = { 3.0f,0.0f,0.0f };
	//球
	Vector3 spherePos1_ = { 0.0f,0.0f,0.0f };
	Vector3 spherePos2_ = { 3.0f,0.0f,0.0f };
	//カメラワーク
	std::unique_ptr<OrbitCameraWork> orbitCameraWork_;
	std::unique_ptr<SplineCamera> splineCamera_;
	std::unique_ptr<FollowCamera> followCamera_;
	std::unique_ptr<TopDownCamera> topDownCamera_;
	//ゲームオブジェクト
	std::unique_ptr<Player> player;
	std::unique_ptr<GameObject> enemy;
	//エミッター
	std::unique_ptr<ParticleEmitter> dust_;
	std::unique_ptr<ParticleEmitter> redEffect_;
	std::unique_ptr<ParticleEmitter> fallHeart_;
	std::unique_ptr<ParticleEmitter> glitch_;
	std::unique_ptr<ParticleEmitter> mordeVFXGround_;
	std::unique_ptr<ParticleEmitter> mordeVFXFragment_;
};
