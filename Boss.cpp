#include "Boss.h"
#include "Camera.h"
#include "PINK.h"
#include <Novice.h>
#include <cmath>
#include <cstdlib>

namespace {
	const float kPi = 3.14f;

	int randomFrame(int minSecond, int maxSecond) {
		int minFrame = minSecond * 60;
		int maxFrame = maxSecond * 60;

		return minFrame +
			rand() % (maxFrame - minFrame + 1);
	}

	const int kScreenWidth = 1280;
	const int kScreenHeight = 720;
}

Boss::Boss() {
	position_ = {
		kBossMapWidth * kTileSize / 2.0f ,
		kBossMapHeight * kTileSize / 2.0f
	};
	radius_ = 30.0f;

	warpTimer_ = randomFrame(5, 9);
	attackTimer_ = randomFrame(4, 7);

	for (int i = 0;i < kMaxBullets;i++) {
		bullets_[i].position = {};
		bullets_[i].velocity = {};
		bullets_[i].radius = 6.0f;
		bullets_[i].isActive = false;
	}
}
void Boss::Update(bool canUseCommonAttack) {

	//Hover中だけワープと全方位弾を更新
	if (canUseCommonAttack) {
		warpTimer_--;
		if (warpTimer_ < -0) {
			Warp();
			warpTimer_ = randomFrame(5, 9);
		}

		attackTimer_--;
		if (attackTimer_ <= 0) {
			FireRadialBullets();
			attackTimer_ = randomFrame(4, 7);
		}
	}

	//全方位弾発射の時間を減らす
	attackTimer_--;
	if (attackTimer_ <= 0) {
		FireRadialBullets();
		attackTimer_ = 120;
	}

	//発射済みの弾は、特殊攻撃中でも移動させる
	for (int i = 0;i < kMaxBullets;i++) {
		if (!bullets_[i].isActive) {
			continue;
		}
		bullets_[i].position.x += bullets_[i].velocity.x;
		bullets_[i].position.y += bullets_[i].velocity.y;

		//画面外に行ったら消滅
		if (bullets_[i].position.x<-bullets_[i].radius ||
			bullets_[i].position.x>
			kBossMapWidth * kTileSize + bullets_[i].radius ||
			bullets_[i].position.y<-bullets_[i].radius ||
			bullets_[i].position.y>
			kBossMapHeight * kTileSize + bullets_[i].radius) {
			bullets_[i].isActive = false;
		}
	}
}

void Boss::Warp() {
	//画面端に寄りすぎない範囲へランダムワープ
	position_.x = static_cast<float>(100 + rand() % 1080);
	position_.y = static_cast<float>(80 + rand() % 300);
}

void Boss::FireRadialBullets() {
	const int kBulletCount = 16;
	const float kBulletSpeed = 4.0f;

	for (int bulletNumber = 0;bulletNumber < kBulletCount;bulletNumber++) {
		//0~360度を等間隔に分割
		float angle =
			2.0f * kPi * static_cast<float>(bulletNumber) /
			static_cast<float>(kBulletCount);

		//未使用の弾を探す
		for (int i = 0;i < kMaxBullets;i++) {
			if (!bullets_[i].isActive) {
				bullets_[i].position = position_;
				bullets_[i].velocity = {
					std::cos(angle) * kBulletSpeed,
					std::sin(angle) * kBulletSpeed
				};
				bullets_[i].radius = 6.0f;
				bullets_[i].isActive = true;
				break;
			}
		}
	}
}

void Boss::ClearBullets() {
	for (int i = 0;i < kMaxBullets;i++) {
		bullets_[i].isActive = false;
		bullets_[i].position = {};
		bullets_[i].velocity = {};
	}
}

Vector2 Boss::GetPosition() const {
	return position_;
}

void Boss::SetPosition(Vector2 position) {
	position_ = position;
}

void Boss::Draw(const Camera& camera)
{
	Vector2 bossScreenPosition =
		camera.WorldToScreen(position_);

	Novice::DrawEllipse(
		static_cast<int>(bossScreenPosition.x),
		static_cast<int>(bossScreenPosition.y),
		static_cast<int>(radius_),
		static_cast<int>(radius_),
		0.0f,
		RED,
		kFillModeSolid
	);


	for (int i = 0; i < kMaxBullets; i++)
	{
		if (!bullets_[i].isActive)
		{
			continue;
		}

		Vector2 bulletScreenPosition =
			camera.WorldToScreen(bullets_[i].position);

		Novice::DrawEllipse(
			static_cast<int>(bulletScreenPosition.x),
			static_cast<int>(bulletScreenPosition.y),
			static_cast<int>(bullets_[i].radius),
			static_cast<int>(bullets_[i].radius),
			0.0f,
			WHITE,
			kFillModeSolid
		);
	}
}