#pragma once
#include <Novice.h>
#include <dinput.h>
#include "Vector2.h"

class InputManager
{
public:
    void Initialize();
    void Update();

    // 移動・攻撃方向
    Vector2 GetMoveDirection() const;
    Vector2 GetAimDirection() const;

    // 接続状態
    bool IsControllerConnected() const;

    // コントローラーボタン
    bool IsButtonDown(int button) const;
    bool IsButtonTrigger(int button) const;

    // キーボード
    bool IsKeyDown(int key) const;
    bool IsKeyTrigger(int key) const;

private:
    // スティックの値を-1～1に変換
    Vector2 GetStickDirection(
        LONG x, LONG y) const;

private:
    char keys_[256] = {};
    char preKeys_[256] = {};

    DIJOYSTATE2 joystickState_ = {};
    DIJOYSTATE2 preJoystickState_ = {};

    bool controllerConnected_ = false;
};
