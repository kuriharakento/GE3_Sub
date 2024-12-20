#include "input/Input.h"

#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input* Input::instance_ = nullptr;

Input* Input::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new Input();
	}
	return instance_;
}

void Input::Finalize()
{
	delete instance_;
	instance_ = nullptr;
}

void Input::Initialize(WinApp* winApp)
{
	//借りてきたWinAppのインスタンスを記録
	winApp_ = winApp;

	HRESULT result;

	//DirectInputのインスタンス生成
	result = DirectInput8Create(
		winApp_->GetHInstance(),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&directInput_,
		nullptr
	);
	assert(SUCCEEDED(result));


	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//排他制御7レベルのセット
	result = keyboard_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update()
{
	HRESULT result;

	///前回のキー情報の保存
	memcpy(keyPre_, key_, sizeof(key_));

	//キーボード情報の取得開始
	result = keyboard_->Acquire();
	//キーボード情報の取得
	result = keyboard_->GetDeviceState(sizeof(key_), key_);
}

bool Input::PushKey(BYTE keyNumber)
{
	if(key_[keyNumber])
	{
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if(key_[keyNumber] && !keyPre_[keyNumber])
	{
		return true;
	}
	return false;
}
