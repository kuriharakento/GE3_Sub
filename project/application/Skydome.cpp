#include "Skydome.h"

void Skydome::Initialize(Object3dCommon* object3dCommon, const std::string& modelName)
{
	skydome_ = std::make_unique<Object3d>();
	skydome_->Initialize(object3dCommon);
	skydome_->SetModel(modelName);

	transform_ = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f },
	};

	skydome_->SetLightingIntensity(0.3f);
}

void Skydome::Update(CameraManager* cameraManager)
{
	skydome_->SetTranslate(transform_.translate);
	skydome_->SetRotate(transform_.rotate);
	skydome_->SetScale(transform_.scale);
	skydome_->Update(cameraManager);
}

void Skydome::Draw()
{
	skydome_->Draw();
}