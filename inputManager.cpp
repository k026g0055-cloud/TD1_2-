#pragma once

#include <Novice.h>
#include <math.h>
#include <Xinput.h>
#include "InputManager.h"
#include "Vector2.h"

#pragma comment(lib, "Xinput.lib")

// 初期化
void InputManager::Initialize()
{
    memset(keys_, 0, sizeof(keys_));
    memset(preKeys_, 0, sizeof(preKeys_));

    joystickState_ = {};
    preJoystickState_ = {};

    controllerConnected_ = false;
}

// 毎フレーム更新
void InputManager::Update()
{
    // キーボード入力
    memcpy(preKeys_, keys_, sizeof(keys_));
    Novice::GetHitKeyStateAll(keys_);

    // 前フレームの入力を保存
    preJoystickState_ = joystickState_;

    // Xboxコントローラーの入力を取得
    joystickState_ = {};

    DWORD result = XInputGetState(
        0,
        &joystickState_
    );

    controllerConnected_ = (result == ERROR_SUCCESS);

    // 未接続なら入力を初期化
    if (!controllerConnected_)
    {
        joystickState_ = {};
        preJoystickState_ = {};
    }
}


// スティック入力を変換
Vector2 InputManager::GetStickDirection(
    SHORT x, SHORT y, SHORT deadZone) const
{
    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);

    float length = sqrtf(fx * fx + fy * fy);

    if (length <= static_cast<float>(deadZone))
    {
        return { 0.0f, 0.0f };
    }

    float dirX = fx / length;
    float dirY = fy / length;

    float power =
        (length - static_cast<float>(deadZone)) /
        (32767.0f - static_cast<float>(deadZone));

    if (power > 1.0f)
    {
        power = 1.0f;
    }

    return {
        dirX * power,
        -dirY * power
    };
}

// 移動方向
Vector2 InputManager::GetMoveDirection() const
{
    if (controllerConnected_)
    {
        return GetStickDirection(
            joystickState_.Gamepad.sThumbLX,
            joystickState_.Gamepad.sThumbLY,
            XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
        );
    }

    Vector2 move = {};

    if (keys_[DIK_W]) move.y -= 1.0f;
    if (keys_[DIK_S]) move.y += 1.0f;
    if (keys_[DIK_A]) move.x -= 1.0f;
    if (keys_[DIK_D]) move.x += 1.0f;

    float length = sqrtf(
        move.x * move.x + move.y * move.y
    );

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
        joystickState_.Gamepad.sThumbRX,
        joystickState_.Gamepad.sThumbRY,
        XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
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
    if (!controllerConnected_)
    {
        return false;
    }

    return (
        joystickState_.Gamepad.wButtons & button
    ) != 0;
}

// ボタンを押した瞬間
bool InputManager::IsButtonTrigger(int button) const
{
    if (!controllerConnected_)
    {
        return false;
    }

    bool current =
        (joystickState_.Gamepad.wButtons & button) != 0;

    bool previous =
        (preJoystickState_.Gamepad.wButtons & button) != 0;

    return current && !previous;
}

// キーを押している間
bool InputManager::IsKeyDown(int key) const
{
    if (key < 0 || key >= 256 || controllerConnected_)
    {
        return false;
    }

    return keys_[key] != 0;
}

// キーを押した瞬間
bool InputManager::IsKeyTrigger(int key) const
{
    if (key < 0 || key >= 256 || controllerConnected_)
    {
        return false;
    }

    return keys_[key] && !preKeys_[key];
}

// デバッグ表示
void InputManager::DrawDebug() const
{
    Novice::ScreenPrintf(
        10, 100,
        "Connected: %d",
        controllerConnected_
    );

    if (controllerConnected_)
    {
        Novice::ScreenPrintf(
            10, 120,
            "Left X:%d Y:%d",
            joystickState_.Gamepad.sThumbLX,
            joystickState_.Gamepad.sThumbLY
        );

        Novice::ScreenPrintf(
            10, 140,
            "Right X:%d Y:%d",
            joystickState_.Gamepad.sThumbRX,
            joystickState_.Gamepad.sThumbRY
        );
    }

    Vector2 move = GetMoveDirection();

    Novice::ScreenPrintf(
        10, 160,
        "Move X:%.2f Y:%.2f",
        move.x,
        move.y
    );
}



