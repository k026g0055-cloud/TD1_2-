#include "Player.h"
#include "Camera.h"
#include "Map.h"

#include <Novice.h>
#include <math.h>

void Player::Initialize()
{
	position_ = { 640.0f, 360.0f };
	size_ = { 48.0f, 48.0f };
	direction_ = { 0.0f, 1.0f };

	speed_ = 5.0f;
}

void Player::Update(const InputManager& input, const Map& map)
{
    // 移動方向
    Vector2 move = input.GetMoveDirection();

    float halfWidth = size_.x / 2.0f;
    float halfHeight = size_.y / 2.0f;

    // X方向の移動
    float nextX = position_.x + move.x * speed_;

    // マップ外に出ないように制限
    if (nextX < halfWidth)
    {
        nextX = halfWidth;
    }

    if (nextX > map.GetWidth() - halfWidth)
    {
        nextX = map.GetWidth() - halfWidth;
    }

    // X方向の壁判定
    if (!map.IsWallAt(nextX - halfWidth, position_.y - halfHeight) &&
        !map.IsWallAt(nextX + halfWidth - 1.0f, position_.y - halfHeight) &&
        !map.IsWallAt(nextX - halfWidth, position_.y + halfHeight - 1.0f) &&
        !map.IsWallAt(nextX + halfWidth - 1.0f, position_.y + halfHeight - 1.0f))
    {
        position_.x = nextX;
    }

    // Y方向の移動
    float nextY = position_.y + move.y * speed_;

    if (nextY < halfHeight)
    {
        nextY = halfHeight;
    }

    if (nextY > map.GetHeight() - halfHeight)
    {
        nextY = map.GetHeight() - halfHeight;
    }

    // Y方向の壁判定
    if (!map.IsWallAt(position_.x - halfWidth, nextY - halfHeight) &&
        !map.IsWallAt(position_.x + halfWidth - 1.0f, nextY - halfHeight) &&
        !map.IsWallAt(position_.x - halfWidth, nextY + halfHeight - 1.0f) &&
        !map.IsWallAt(position_.x + halfWidth - 1.0f, nextY + halfHeight - 1.0f))
    {
        position_.y = nextY;
    }

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
}

void Player::ClampToArea(float areaWidth, float areaHeight)
{
	const float halfWidth = size_.x / 2.0f;
	const float halfHeight = size_.y / 2.0f;

	if (position_.x < halfWidth)
	{
		position_.x = halfWidth;
	}
	else if (position_.x > areaWidth - halfWidth)
	{
		position_.x = areaWidth - halfWidth;
	}

	if (position_.y < halfHeight)
	{
		position_.y = halfHeight;
	}
	else if (position_.y > areaHeight - halfHeight)
	{
		position_.y = areaHeight - halfHeight;
	}
}


void Player::Draw(const Camera& camera) const
{
	// ワールド座標からスクリーン座標へ
	Vector2 screenPosition = camera.WorldToScreen(position_);

	float drawX = screenPosition.x - size_.x / 2.0f;
	float drawY = screenPosition.y - size_.y / 2.0f;

	// プレイヤー描画
	Novice::DrawBox(
		static_cast<int>(drawX),
		static_cast<int>(drawY),
		static_cast<int>(size_.x),
		static_cast<int>(size_.y),
		0.0f, BLUE, kFillModeSolid
	);

	// 向き表示
	Novice::DrawLine(
		static_cast<int>(screenPosition.x),
		static_cast<int>(screenPosition.y),
		static_cast<int>(screenPosition.x + direction_.x * 40.0f),
		static_cast<int>(screenPosition.y + direction_.y * 40.0f),
		WHITE
	);
}

Vector2 Player::GetPosition() const
{
	return position_;
}

void Player::SetPosition(Vector2 position) {
	position_ = position;
}

void Player::Debug()
{
	Novice::ScreenPrintf(10, 30, "position {%.2f : %.2f}", position_.x, position_.y);
}
