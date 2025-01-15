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

	/// \brief マウスの移動量（X方向）を取得
	/// \return 移動量
	long GetMouseDeltaX() const;

	/// \brief マウスの移動量（Y方向）を取得
	/// \return 移動量
	long GetMouseDeltaY() const;

	/// \brief マウスホイールの移動量を取得
	/// \return 移動量
	long GetMouseDeltaWheel() const;

	/// \brief マウスボタンの押下をチェック
	/// \param buttonNumber ボタン番号（0: 左, 1: 右, 2: 中央）
	/// \return 押されているか
	bool PushMouseButton(int buttonNumber) const;

	/// \brief マウスボタンのトリガーをチェック
	/// \param buttonNumber ボタン番号（0: 左, 1: 右, 2: 中央）
	/// \return トリガーか
	bool TriggerMouseButton(int buttonNumber) const;

private: //メンバ変数
	//WindowsAPI
	WinApp* winApp_ = nullptr;

	//キーボードデバイス
	ComPtr<IDirectInputDevice8> keyboard_;

	//マウス
	ComPtr<IDirectInputDevice8> mouse_;

	//DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput_;

	//全キーの入力情報を取得する
	BYTE key_[256] = {};

	//前回の全キーの状態
	BYTE keyPre_[256] = {};

	// マウスの現在の状態
	DIMOUSESTATE mouseState_ = {};

	// マウスの前回の状態
	DIMOUSESTATE mouseStatePre_ = {};

private: //シングルトン
	static Input* instance_;

	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
};

