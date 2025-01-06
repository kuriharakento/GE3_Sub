#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>
#include "IScene.h"


class SpriteCommon;
class CameraManager;
class Object3dCommon;

class SceneManager
{
public:
    SceneManager() = default;
    ~SceneManager() = default;

    // シーンを登録
    template<typename T>
    void RegisterScene(const std::string& name) {
        sceneFactories_[name] = [this]() { return std::make_unique<T>(this); };
    }

    // シーンを変更
    void ChangeScene(const std::string& sceneName);

    void Initialize(Object3dCommon* objectCommon, CameraManager* camera,SpriteCommon* spriteCommon);

    // 現在のシーンを更新
    void Update();

    // 現在のシーンを描画
	void Draw3D();
	void Draw2D();

	std::string GetCurrentSceneName() const { return currentSceneName_; }

private:
	void NextScene();

private:
    //ポインタ
	Object3dCommon* object3dCommon_ = nullptr;
	CameraManager* cameraManager_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;

    std::unordered_map<std::string, std::function<std::unique_ptr<IScene>()>> sceneFactories_;
    std::unique_ptr<IScene> currentScene_;
    std::string currentSceneName_;
};

