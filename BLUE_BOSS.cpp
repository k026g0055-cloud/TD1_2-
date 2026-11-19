#include "BLUE_BOSS.h"
#include "Boss.h"
#include <Novice.h>
#include <cmath>
#include <cstdlib>


//Pull確認用散歩怪異
namespace {
	const float kPi = 3.14f;
	const int kScreenWidth = 1280;
	const int kScreenHeight = 720;
}

Blue_Boss::Blue_Boss() {
	position_ = { 640.0f,150.0f };
	radius_ = 30.0f;

	warpTimer_ = 180;	// ワープタイマー
	attackTimer_ = 120;	// 攻撃タイマー

	for (int i = 0;i < kMaxBullets;i++) {
		bullets_[i].position = {};
		bullets_[i].velocity = {};
		bullets_[i].radius = 6.0f;
		bullets_[i].isActive = false;
	}
}
void Blue_Boss::Update() {

	//ワープまでの時間を減らす
	warpTimer_--;
	if (warpTimer_ <= 0) {
		Warp();
		warpTimer_ = 180;
	}

	//全方位弾発射の時間を減らす
	attackTimer_--;
	if (attackTimer_ <= 0) {
		FireRadialBullets();
		attackTimer_ = 120;
	}

	//弾の移動
	for (int i = 0;i < kMaxBullets;i++) {
		if (!bullets_[i].isActive) {
			continue;
		}
		bullets_[i].position.x += bullets_[i].velocity.x;
		bullets_[i].position.y += bullets_[i].velocity.y;

		//画面外に出たら弾を消す
		if (bullets_[i].position.x<-bullets_[i].radius ||
			bullets_[i].position.x>kScreenWidth + bullets_[i].radius ||
			bullets_[i].position.y<-bullets_[i].radius ||
			bullets_[i].position.y>kScreenHeight + bullets_[i].radius) {
			bullets_[i].isActive = false;
		}
	}
}

void Blue_Boss::Warp() {
	//画面端に寄りすぎない範囲へランダムワープ
	position_.x = static_cast<float>(100 + rand() % 1080);
	position_.y = static_cast<float>(80 + rand() % 300);
}

void Blue_Boss::FireRadialBullets() {
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

void Blue_Boss::Draw() {
	//ボス
	Novice::DrawEllipse(
		static_cast<int>(position_.x),
		static_cast<int>(position_.y),
		static_cast<int>(radius_),
		static_cast<int>(radius_),
		0.0f,
		BLUE,
		kFillModeSolid
	);

	//弾
	for (int i = 0;i < kMaxBullets;i++) {
		if (bullets_[i].isActive) {
			Novice::DrawEllipse(
				static_cast<int>(bullets_[i].position.x),
				static_cast<int>(bullets_[i].position.y),
				static_cast<int>(bullets_[i].radius),
				static_cast<int>(bullets_[i].radius),
				0.0f,
				WHITE,
				kFillModeSolid
			);
		}
	}
}