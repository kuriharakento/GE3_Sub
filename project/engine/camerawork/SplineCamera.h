#pragma once
#include "camerawork/CameraWorkBase.h"
#include "camerawork/spline/SplineData.h"

class SplineCamera : public CameraWorkBase
{
public:
	void Initialize(Camera* camera) override;
	void Update() override;
	void Start(float speed, bool loop);
	void LoadJson(const std::string& filePath);
	void SetLoop(bool loop) { loop_ = loop; }
	void SetTarget(const Vector3* target) { targetPtr_ = target; }

private:
	Camera* camera_ = nullptr;	//カメラ
	SplineData splineData_;	//スプラインデータ
	const Vector3* targetPtr_ = nullptr;	//ターゲットのポインタ
	float time_ = 0.0f;	//時間経過
	float speed_ = 0.0f;	//スピード
	bool loop_ = false;	//ループフラグ
};

