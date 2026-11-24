#pragma once
#include "Vector2.h"
#include "GameConfig.h"

class Camera;

class Boss
{
public:
	static const int kMaxBullets = 100;	// 最大弾数

	struct Bullet {
		Vector2 position;
		Vector2 velocity;
		float radius;
		bool isActive;
	};

	Boss();
	void Update(bool canUseCommonAttack=true);
	void Draw(const Camera& camera);

protected:
	void ClearBullets();
	Vector2 GetPosition() const;
	void SetPosition(Vector2 positoin);

private:
	void Warp();
	void FireRadialBullets();

	Vector2 position_ = {
		25.0f*kTileSize,
		5.0f * kTileSize
	};
	float radius_;

	int warpTimer_;
	int attackTimer_;

	Bullet bullets_[kMaxBullets];
};

