#pragma once
#include <Windows.h>
#include <wrl.h>

#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include <dinput.h>

#include "base/WinApp.h"

class Input
{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //メンバ関数
	//シングルトン
	static Input* GetInstance();
	//終了
	void Finalize();

	//初期化
	void Initialize(WinApp* winApp);
	//更新
	void Update();

	/// \brief キーの押下をチェック
	/// \param keyNumber キー番号
	/// \return 押されているか
	bool PushKey(BYTE keyNumber);

	/// \brief キーのトリガーをチェック
	/// \param keyNumber キー番号
	/// \return トリガーか
	bool TriggerKey(BYTE keyNumber);

private: //メンバ変数
	//WindowsAPI
	WinApp* winApp_ = nullptr;

	//キーボードデバイス
	ComPtr<IDirectInputDevice8> keyboard_;

	//DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput_;

	//全キーの入力情報を取得する
	BYTE key_[256] = {};

	//前回の全キーの状態
	BYTE keyPre_[256] = {};

private: //シングルトン
	static Input* instance_;

	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
};

