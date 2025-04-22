#pragma once
#include "CameraWorkBase.h"

class OrbitCameraWork : public CameraWorkBase
{
public: //メンバ関数
	//初期化
	void Initialize(Camera* camera) override;
	//更新
	void Update() override;
	//ターゲットを指定して開始
	void Start(Vector3 target, float radius, float speed);
	//ターゲットのポインタを指定して開始
	void Start(Vector3* target, float radius, float speed);
	//終了
	void Stop() { isActive_ = false; }
public:	//アクセッサ
	bool IsActive() const { return isActive_; }
	void SetActive(bool active) { isActive_ = active; }
	void SetTarget(Vector3 target) { targetValue_ = target; }
	void SetTarget(Vector3* target) { targetPtr_ = target; }
private:
	Vector3 targetValue_ = {}; //ターゲットの静的な位置
	Vector3* targetPtr_ = nullptr; //ターゲットの動的な位置
	float radius_; //半径
	float speed_; //回転速度
	float time_ = 0.0f; //時間経過
	bool isActive_ = false; //動作フラグ
};

