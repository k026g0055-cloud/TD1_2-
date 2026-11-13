
#pragma once

#include <Novice.h>
#include <Xinput.h>
#include "Vector2.h"

class InputManager
{
public:
    void Initialize();
    void Update();

    Vector2 GetMoveDirection() const;
    Vector2 GetAimDirection() const;

    bool IsControllerConnected() const;

    bool IsButtonDown(int button) const;
    bool IsButtonTrigger(int button) const;

    bool IsKeyDown(int key) const;
    bool IsKeyTrigger(int key) const;

    void DrawDebug() const;

private:
    Vector2 GetStickDirection(
        SHORT x, SHORT y, SHORT deadZone) const;

    char keys_[256] = {};
    char preKeys_[256] = {};

    XINPUT_STATE joystickState_ = {};
    XINPUT_STATE preJoystickState_ = {};

    bool controllerConnected_ = false;
};
