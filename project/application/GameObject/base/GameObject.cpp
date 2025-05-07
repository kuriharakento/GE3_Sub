#include "GameObject.h"

#include "base/Logger.h"

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
	for (auto& [name, comp] : components_) {
		comp->Update(this);  // 依存性を必要最小限に
	}
}

void GameObject::Draw(CameraManager* camera)
{
	if (!object3d_) { return; }

	ApplyTransformToObject3D(camera);

	object3d_->Draw();
}

void GameObject::AddComponent(const std::string& name, std::shared_ptr<IGameObjectComponent> comp)
{
	//すでに同じ名前のコンポーネントが存在する場合はメッセージを出力
	if (components_.find(name) != components_.end()) {
		Logger::Log("Waring: Component not found: " + name);
		return;
	}
	// コンポーネントを追加
	components_[name] = std::move(comp);
}

std::shared_ptr<IGameObjectComponent> GameObject::GetComponent(const std::string& name)
{
	auto it = components_.find(name);
	if (it != components_.end()) {
		return it->second;
	}
	// 見つからなかった場合はnullptrを返す
	Logger::Log("Warnig: Component not found: " + name);
	return nullptr; 
}

//std::shared_ptr<CollisionComponent> GameObject::GetCollisionComponent()
//{
//	for (const auto& [name, comp] : components_) {
//		if (auto collisionComp = std::dynamic_pointer_cast<CollisionComponent>(comp)) {
//			return collisionComp;
//		}
//	}
//	return nullptr;
//}

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