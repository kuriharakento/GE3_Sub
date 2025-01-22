#pragma once

#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include <dinput.h>

#include <Xinput.h>
#pragma comment(lib, "Xinput.lib")

#include <wrl.h>
#include <array>
#include <unordered_map>
#include <vector>
#include <functional>

public: //メンバ関数
	//シングルトン
	static Input* GetInstance();
	//終了
	void Finalize();

    // 初期化
    void Initialize(WinApp* winApp);

    // 終了処理
    void Finalize();

    // 更新
    void Update();

    // キーの押下チェック
    bool PushKey(BYTE keyNumber) const;

    // キーのトリガーチェック
    bool TriggerKey(BYTE keyNumber) const;

    // キーのリリースチェック
    bool ReleaseTrigger(BYTE keyNumber) const;

    // ゲームパッドボタンのリリースチェック
    bool ReleaseButton(DWORD gamepadIndex, DWORD buttonCode) const;

    // デッドゾーンの設定
    void SetDeadZone(float deadZone);

    // ゲームパッドの振動設定
    void SetVibration(DWORD gamepadIndex, WORD leftMotor, WORD rightMotor);

    // ボタンのリマッピング
    void RemapButton(Action action, InputType type, DWORD code);

    // アクションのバインディング
    void BindAction(Action action, std::function<void()> callback);

    // 入力の記録開始
    void StartRecording();

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

    // 入力の再生開始
    void PlayRecording();

	//マウス
	ComPtr<IDirectInputDevice8> mouse_;

	//DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput_;

	//ボタンの押下チェック
    bool IsButtonPressed(DWORD gamepadIndex, DWORD buttonCode) const;

	// ボタンのトリガーチェック
    bool IsButtonTriggered(DWORD gamepadIndex, DWORD buttonCode) const;

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

    // 入力の記録
    bool isRecording_;
    bool isPlaying_;
    std::vector<std::pair<Action, DWORD>> recordedInputs_;
    size_t playIndex_;
};
