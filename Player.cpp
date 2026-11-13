#include "Player.h"

#include <Novice.h>
#include <math.h>

void Player::Initialize()
{
    position_ = { 640.0f, 360.0f };
    size_ = { 48.0f, 48.0f };
    direction_ = { 0.0f, 1.0f };

    speed_ = 5.0f;
}

void Player::Update(const InputManager& input)
{
    // 移動方向を取得
    Vector2 move = input.GetMoveDirection();

    position_.x += move.x * speed_;
    position_.y += move.y * speed_;

    // 右スティックの入力
    Vector2 aim = input.GetAimDirection();

    float aimLength = sqrtf(
        aim.x * aim.x + aim.y * aim.y
    );

    if (aimLength > 0.01f)
    {
        direction_.x = aim.x / aimLength;
        direction_.y = aim.y / aimLength;
    }
    else
    {
        float moveLength = sqrtf(
            move.x * move.x + move.y * move.y
        );

        if (moveLength > 0.01f)
        {
            direction_.x = move.x / moveLength;
            direction_.y = move.y / moveLength;
        }
    }

    // 画面外への移動を制限
    float halfWidth = size_.x / 2.0f;
    float halfHeight = size_.y / 2.0f;

    if (position_.x < halfWidth)
    {
        position_.x = halfWidth;
    }

    if (position_.x > 1280.0f - halfWidth)
    {
        position_.x = 1280.0f - halfWidth;
    }

    if (position_.y < halfHeight)
    {
        position_.y = halfHeight;
    }

    if (position_.y > 720.0f - halfHeight)
    {
        position_.y = 720.0f - halfHeight;
    }
}

void Player::Draw() const
{
    // 中心座標から左上座標に変換
    float drawX = position_.x - size_.x / 2.0f;
    float drawY = position_.y - size_.y / 2.0f;

    // プレイヤー本体
    Novice::DrawBox(
        static_cast<int>(drawX),
        static_cast<int>(drawY),
        static_cast<int>(size_.x),
        static_cast<int>(size_.y),
        0.0f,
        BLUE,
        kFillModeSolid
    );

    // 向いている方向
    Novice::DrawLine(
        static_cast<int>(position_.x),
        static_cast<int>(position_.y),
        static_cast<int>(position_.x + direction_.x * 40.0f),
        static_cast<int>(position_.y + direction_.y * 40.0f),
        WHITE
    );
}
