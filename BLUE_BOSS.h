#pragma once
#include "Vector2.h"


class Blue_Boss
{
	public:
		static const int kMaxBullets = 100;	// 最大弾数

		struct Bullet {
			Vector2 position;
			Vector2 velocity;
			float radius;
			bool isActive;
		};
		
		Blue_Boss();
		void Update();
		void Draw();

	private:
		void Warp();
		void FireRadialBullets();

		Vector2 position_;
		float radius_;

		int warpTimer_;
		int attackTimer_;

		Bullet bullets_[kMaxBullets];
	};

