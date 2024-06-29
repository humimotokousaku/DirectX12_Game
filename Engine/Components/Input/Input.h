#pragma once

#define DIRECTINPUT_VERSION		0x0800  // DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include "../../base/WinApp.h"
#include <array>
#include <vector>
#include <windows.h>
#include <Xinput.h>

class Input
{
public:
	/// 
	/// Default Method
	/// 
	static Input* GetInstance();

	// 初期化
	void Initialize();

	// 更新処理
	void Update();

	///
	/// Keyboard Method
	/// 
	// キーを押した瞬間
	bool TriggerKey(BYTE keyNumber) const;
	// キーを押している間
	bool PressKey(BYTE keyNumber)const;
	// キーを離した瞬間
	bool ReleaseKey(BYTE keyNumber)const;
	
	/// <summary>
	/// 何もキーが入力されていないかを検出
	/// </summary>
	/// <param name="detectKeys">入力を検知したいキー</param>
	/// <returns>入力なし:true</returns>
	bool DetectKeyInput(std::vector<int> detectKeys);
	/// <summary>
	/// 特定のキーが入力押されていないかを検出
	/// </summary>
	/// <param name="detectKey">入力を検知したいキー</param>
	/// <returns>入力なし:true</returns>
	bool DetectKeyInput(int detectKey);

	///
	/// GamePad Method
	/// 
	// ゲームパッドの状態を取得
	bool GetJoystickState(int32_t stickNo, XINPUT_STATE& state);

	// デッドゾーンを適用する関数
	SHORT ApplyDeadzone(SHORT inputValue);

	// トリガー処理
	bool GamePadTrigger(int GAMEPAD_NUM);
	// リリース処理
	bool GamePadRelease(int GAMEPAD_NUM);
	// 押してるときの処理
	bool GamePadPress(int GAMEPAD_NUM);
	// Lスティックが何も入力されていないかを検出
	// 何も入力されてないならtrue
	bool DetectThumbInput(SHORT sThumbX, SHORT sThumbY);

private:
	/// 
	/// Keyboard
	/// 
	IDirectInput8* directInput_ = nullptr;
	IDirectInputDevice8* keyboard_ = nullptr;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> preKey_;

	///
	/// GamePad
	/// 
	// デッドゾーンの閾値
	const int DEADZONE_THRESHOLD = 8000;

	XINPUT_STATE joyState_;
	XINPUT_STATE preJoyState_;
};

