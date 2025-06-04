#pragma once
#include <memory>
#include <string>

#include "3d/Object3d.h"
#include "application/GameObject/component/base/IGameObjectComponent.h"
#include "base/GraphicsTypes.h"

class GameObject
{
public:
	virtual ~GameObject();
	explicit GameObject(std::string tag = "");	// コンストラクタ
	virtual void Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, Camera* camera = nullptr);		// 初期化
	virtual void Update();
	virtual void Draw(CameraManager* camera);
	void AddComponent(const std::string& name, std::unique_ptr<IGameObjectComponent> comp);	// コンポーネントの追加
	template<typename T>
	std::shared_ptr<T> GetComponent() const;
public: //アクセッサ
	//トランスフォーム
	virtual void SetPosition(const Vector3& pos) { transform_.translate = pos; }
	virtual void SetRotation(const Vector3& rot) { transform_.rotate = rot; }
	virtual void SetScale(const Vector3& scale) { transform_.scale = scale; }
	virtual const Vector3& GetPosition() const { return transform_.translate; }
	virtual const Vector3& GetRotation() const { return transform_.rotate; }
	virtual const Vector3& GetScale() const { return transform_.scale; }

	//オブジェクト3D
	void SetModel(const std::string& modelName) { object3d_->SetModel(modelName); }	// モデルの設定

	//タグ
	std::string GetTag() const { return tag_; }	// タグの取得
	void SetTag(const std::string& tag) { tag_ = tag; }	// タグの設定

protected:
	Transform transform_;																	// Transform情報
	std::unique_ptr<Object3d> object3d_;													// 3Dオブジェクト

private:
	void ApplyTransformToObject3D(CameraManager* camera);											// Transform情報をObject3Dに適用

private:
	std::unordered_map<std::string, std::shared_ptr<IGameObjectComponent>> components_;		// コンポーネントのリスト
	std::string tag_; 																		// オブジェクトのタグ
};

template <typename T>
std::shared_ptr<T> GameObject::GetComponent() const
{
	for (const auto& [_, comp] : components_)
	{
		if (auto casted = std::dynamic_pointer_cast<T>(comp))
		{
			return casted;
		}
	}
	return nullptr;
}