#pragma once
#include "2d/SpriteCommon.h"
#include "3d/Object3dCommon.h"
#include "base/DirectXCommon.h"
#include "base/WinApp.h"
#include "manager/CameraManager.h"
#include "manager/ImGuiManager.h"
#include "manager/SrvManager.h"

class MyGame
{
public:
	//初期化
	void Initialize();

	//終了
	void Finalize();

	//毎フレーム
	void Update();

	//描画
	void Draw();

public: //アクセッサ
	//ゲームの終了フラグ
	bool IsEndRequest() const { return endRequest_; }

private:	//汎用機能
	//ウィンドウアプリケーション
	WinApp* winApp_;
	//DirectXCommon
	DirectXCommon* dxCommon_;
	//SRVマネージャー
	std::unique_ptr<SrvManager> srvManager_;
	//ImGuiManager
	ImGuiManager* imguiManager_;
	//スプライト共通部
	SpriteCommon* spriteCommon_;
	//3Dオブジェクト共通部
	Object3dCommon* objectCommon_;
	//カメラマネージャー
	std::unique_ptr<CameraManager> cameraManager_;

private: //メンバ変数
	//ゲームの終了フラグ
	bool endRequest_ = false;

};

