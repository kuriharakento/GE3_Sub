#include "ModelManager.h"

ModelManager* ModelManager::instance_ = nullptr;

ModelManager* ModelManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ModelManager();
	}
	return instance_;
}

void ModelManager::Initialize(DirectXCommon* dxCommon)
{
	modelCommon_ = new ModelCommon();
	modelCommon_->Initialize(dxCommon);
}

void ModelManager::Finalize()
{
	if (instance_ != nullptr)
	{
		delete modelCommon_;
		delete instance_;
		instance_ = nullptr;
	}
}
