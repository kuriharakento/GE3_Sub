#pragma once
#include "2d/SpriteCommon.h"
#include "3d/Object3dCommon.h"
#include "application/scene/factory/SceneFactory.h"
#include "application/scene/manager/SceneManager.h"
#include "base/DirectXCommon.h"
#include "base/WinApp.h"
#include "manager/CameraManager.h"
#include "manager/ImGuiManager.h"
#include "manager/SrvManager.h"

class Framework
{
public: //メンバ関数
	//デストラクタ
	virtual ~Framework() = default;
	//初期化
	virtual void Initialize();
	//終了
	virtual void Finalize();
	//毎フレーム
	virtual void Update();
	//更新後の処理
	void PostUpdate();
	//描画
	virtual void Draw() = 0;
	//3D描画用の設定
	void Draw3DSetting();
	//2D描画用の設定
	void Draw2DSetting();

	//描画前処理
	void PreDraw();
	//描画後処理
	void PostDraw();
	//終了リクエストがあるか
	virtual bool IsEndRequest() { return endRequest_; }

	//実行
	void Run();

protected: //メンバ変数
	//終了リクエスト
	bool endRequest_ = false;

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
	//シーンマネージャー
	std::unique_ptr<SceneManager> sceneManager_;
	//シーンファクトリ
	std::unique_ptr<SceneFactory> sceneFactory_;
};

