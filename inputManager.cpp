#include "InputManager.h"
#include <math.h>
#include <string.h>

// 初期化
void InputManager::Initialize()
{
	memset(keys_, 0, sizeof(keys_));
	memset(preKeys_, 0, sizeof(preKeys_));

	joystickState_ = {};
	preJoystickState_ = {};

	controllerConnected_ = false;

	// 左右スティックのデッドゾーン
	Novice::SetJoystickDeadZone(0, 5000, 5000);
}

// 毎フレーム更新

void InputManager::Update()
{
	// 前フレームのキーボード入力を保存
	memcpy(preKeys_, keys_, sizeof(keys_));

	// 現在のキーボード入力を取得
	Novice::GetHitKeyStateAll(keys_);

	// 前フレームのコントローラー入力を保存
	preJoystickState_ = joystickState_;

	// 現在のコントローラー入力を取得
	controllerConnected_ = (Novice::GetJoystickState(0, &joystickState_) == 0);

	// 接続されていなければ初期化
	if (!controllerConnected_)
	{
		joystickState_ = {};
		preJoystickState_ = {};
	}
}


// スティック入力を変換
Vector2 InputManager::GetStickDirection(
	LONG x, LONG y) const
{
	// DirectInputの中心値は32767付近
	float fx =(static_cast<float>(x) - 32767.0f) / 32767.0f;

	float fy =(static_cast<float>(y) - 32767.0f) / 32767.0f;

	// 入力の大きさを取得
	float length = sqrtf(fx * fx + fy * fy);

	if (length <= 0.0f)
	{
		return { 0.0f, 0.0f };
	}

	// 斜め方向が速くならないようにする
	if (length > 1.0f)
	{
		fx /= length;
		fy /= length;
	}

	return { fx, fy };
}

// 移動方向
Vector2 InputManager::GetMoveDirection() const
{
	if (controllerConnected_)
	{
		Vector2 stick = GetStickDirection(
			joystickState_.lX,
			joystickState_.lY
		);

		if (stick.x != 0.0f || stick.y != 0.0f)
		{
			return stick;
		}
	}

	Vector2 move = {};

	if (keys_[DIK_W]) move.y -= 1.0f;
	if (keys_[DIK_S]) move.y += 1.0f;
	if (keys_[DIK_A]) move.x -= 1.0f;
	if (keys_[DIK_D]) move.x += 1.0f;

	float length = sqrtf(
		move.x * move.x + move.y * move.y);

	if (length > 0.0f)
	{
		move.x /= length;
		move.y /= length;
	}

	return move;
}

// 攻撃方向
Vector2 InputManager::GetAimDirection() const
{
	if (!controllerConnected_)
	{
		return { 0.0f, 0.0f };
	}

	return GetStickDirection(
		joystickState_.lRx,
		joystickState_.lRy
	);
}

// 接続確認
bool InputManager::IsControllerConnected() const
{
	return controllerConnected_;
}

// ボタンを押している間
bool InputManager::IsButtonDown(int button) const
{
	if (!controllerConnected_ ||
		button < 0 || button >= 128)
	{
		return false;
	}

	return (joystickState_.rgbButtons[button] & 0x80) != 0;
}

// ボタンを押した瞬間
bool InputManager::IsButtonTrigger(int button) const
{
	if (!controllerConnected_ ||
		button < 0 || button >= 128)
	{
		return false;
	}

	bool current =
		(joystickState_.rgbButtons[button] & 0x80) != 0;

	bool previous =
		(preJoystickState_.rgbButtons[button] & 0x80) != 0;

	return current && !previous;
}

// キーを押している間
bool InputManager::IsKeyDown(int key) const
{
	if (key < 0 || key >= 256)
	{
		return false;
	}

	return keys_[key] != 0;
}

// キーを押した瞬間
bool InputManager::IsKeyTrigger(int key) const
{
	if (key < 0 || key >= 256)
	{
		return false;
	}

	return keys_[key] && !preKeys_[key];
}



