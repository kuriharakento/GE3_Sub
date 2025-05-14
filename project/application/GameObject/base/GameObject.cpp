#include "GameObject.h"

#include "application/GameObject/component/base/IActionComponent.h"
#include "base/Logger.h"

GameObject::GameObject(std::string tag)
{
	// タグの初期化
	assert(!tag.empty() && "ERROR: GameObject::GameObject() - Tag should not be empty. Ensure that you provide a valid tag.");
	tag_ = tag;
}

void GameObject::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, Camera* camera)
{
	// 3Dオブジェクトの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(object3dCommon, camera);
	object3d_->SetModel("cube.obj");
	object3d_->SetLightManager(lightManager);
	// Transformの初期化
	transform_ = {
		{ 1.0f, 1.0f, 1.0f }, // scale
		{ 0.0f, 0.0f, 0.0f }, // rotate
		{ 0.0f, 0.0f, 0.0f }  // translate
	};
	// コンポーネントの初期化
	components_.clear();
}

void GameObject::Update()
{
	// コンポーネントを更新
	for (auto& [name, comp] : components_)
	{
		comp->Update(this); // コンポーネントの更新
	}
}

void GameObject::Draw(CameraManager* camera)
{
	if (!object3d_) { return; }

	// Transform情報をObject3Dに適用
	ApplyTransformToObject3D(camera);

	// 3Dオブジェクトの描画
	object3d_->Draw();

	// コンポーネントを更新
	for (auto& [name, comp] : components_)
	{
		// IActionComponent にキャスト可能か確認
		if (auto actionComp = std::dynamic_pointer_cast<IActionComponent>(comp))
		{
			actionComp->Draw(camera); // アクションコンポーネントの描画
		}
	}
}

void GameObject::AddComponent(const std::string& name, std::shared_ptr<IGameObjectComponent> comp)
{
	//すでに同じ名前のコンポーネントが存在する場合はメッセージを出力
	if (components_.find(name) != components_.end())
	{
		Logger::Log("Warning: Component already exists: " + name);
	}
	// コンポーネントを追加
	components_[name] = std::move(comp);
}

void GameObject::ApplyTransformToObject3D(CameraManager* camera)
{
	if (!object3d_) { return; }

	// Transform情報をObject3Dに適用
	object3d_->SetTranslate(transform_.translate);
	object3d_->SetRotate(transform_.rotate);
	object3d_->SetScale(transform_.scale);

	//行列の更新
	object3d_->Update(camera);
}