#pragma once
#include "2d/SpriteCommon.h"
#include "3d/Object3dCommon.h"
#include "base/PostProcessPass.h"
#include "engine/scene/factory/SceneFactory.h"
#include "engine/scene/manager/SceneManager.h"
#include "base/DirectXCommon.h"
#include "base/PostProcessManager.h"
#include "base/RenderTexture.h"
#include "base/WinApp.h"
#include "lighting/LightManager.h"
#include "line/LineManager.h"
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
	//パフォーマンス情報の表示
	void ShowPerformanceInfo();
	//終了リクエストがあるか
	virtual bool IsEndRequest() { return endRequest_; }
	//実行
	void Run();

protected: //メンバ変数
	//終了リクエスト
	bool endRequest_ = false;
	//ウィンドウアプリケーション
	std::unique_ptr<WinApp> winApp_;
	//DirectXCommon
	std::unique_ptr<DirectXCommon> dxCommon_;
	//SRVマネージャー
	std::unique_ptr<SrvManager> srvManager_;
	//ImGuiManager
	std::unique_ptr<ImGuiManager> imguiManager_;
	//スプライト共通部
	std::unique_ptr<SpriteCommon> spriteCommon_;
	//3Dオブジェクト共通部
	std::unique_ptr<Object3dCommon> objectCommon_;
	//カメラマネージャー
	std::unique_ptr<CameraManager> cameraManager_;
	//シーンマネージャー
	std::unique_ptr<SceneManager> sceneManager_;
	//シーンファクトリ
	std::unique_ptr<SceneFactory> sceneFactory_;
	//ライトマネージャー
	std::unique_ptr<LightManager> lightManager_;
	//
	std::unique_ptr<RenderTexture> renderTexture_;
	//
	std::unique_ptr<PostProcessManager> postProcessManager_;
};

