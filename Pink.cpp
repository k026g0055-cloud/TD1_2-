#include "Pink.h"
#include "Camera.h"
#include "Boss.h"

#include <Novice.h>
#include <cstdlib>
#include <cmath>

Pink::Pink() {
	state_.current = State::Hover;
	state_.timer = 0;

	barrier_.hp = 3;
	barrier_.isActive = false;
	barrier_.animationTimer = 0;

	moveTarget_ = GetPosition();
	moveSpeed_ = 2.0f;
	moveTimer_ = 0;
}

void Pink::Update(Vector2 playerPosition) {
	//先に現在の特殊攻撃状態を更新
	UpdateState(playerPosition);

	//Hover中だけワープと全方位弾を許可
	bool canUseCommonAttack =
		state_.current == State::Hover;
	//弾の移動処理自体は常に実行される
	Boss::Update(canUseCommonAttack);
}

void Pink::Draw(const Camera& camera) {
	Boss::Draw(camera);
	DrawAttack(camera);
	DrawBarrier(camera);
}

void Pink::ChangeState(State nextState) {
	state_.current = nextState;
	state_.timer = 0;
	if (nextState == State::Stunned) {
		barrier_.isActive = false;
		ClearBullets();
	}
}

void Pink::ActivateBarrier() {
	if (barrier_.isActive) {
		return;
	}

	barrier_.hp = 3;
	barrier_.isActive = true;
	barrier_.animationTimer = 0;
}

void Pink::DamageBarrier(int damage) {
	if (!barrier_.isActive || damage <= 0) {
		return;
	}

	barrier_.hp -= damage;

	if (barrier_.hp <= 0) {
		barrier_.hp = 0;
		barrier_.isActive = false;
		ChangeState(State::Stunned);
	}
}

bool Pink::IsBarrierActive() const {
	return barrier_.isActive;
}

void Pink::PrepareBeam(Vector2 playerPosition) {
	Vector2 bossPosition = GetPosition();

	Vector2 differnce = {
		playerPosition.x - bossPosition.x,
		playerPosition.y - bossPosition.y
	};

	float length = std::sqrt(
		differnce.x * differnce.x +
		differnce.y * differnce.y
	);

	if (length > 0.001f) {
		beam_.direction.x = differnce.x / length;
		beam_.direction.y = differnce.y / length;
	} else {
		beam_.direction = { 0.0f,1.0f };
	}
}

void Pink::UpdateState(Vector2 playerPosition) {
	state_.timer++;
	switch (state_.current) {
	case State::Hover:
		if (state_.timer >= 90) {
			const int attackCount = barrier_.isActive ? 4 : 5;
			int attackNumber = rand() % attackCount;
			switch (attackNumber)
			{
			case 0:
				ChangeState(State::Magma);
				break;
			case 1:
				PrepareBeam(playerPosition);
				ChangeState(State::BeamWarning);
				break;
			case 2:
				ChangeState(State::FlameThrower);
				break;
			case 3:
				ChangeState(State::Strike);
				break;
			case 4:
				ActivateBarrier();
				state_.timer = 0;
				break;
			}
		}
		break;

	case State::Magma:
		//マグマ攻撃
		if (state_.timer >= 100) {
			ChangeState(State::Hover);
		}
		break;

	case State::BeamWarning:
		//1秒間のビーム予告
		if (state_.timer >= 60) {
			ChangeState(State::Beam);
		}
		break;
	case State::Beam:
		//1.5秒間ビーム発射
		if (state_.timer >= 90) {
			ChangeState(State::Hover);
		}
		break;
	case State::FlameThrower:
		//2秒間火炎放射
		if (state_.timer >= 120) {
			ChangeState(State::Hover);
		}
		break;
	case State::Strike:
		//打撃は短め
		if (state_.timer >= 45) {
			ChangeState(State::Hover);
		}
		break;
	case State::Stunned:
		//バリア破壊後、3秒停止
		if (state_.timer >= 180) {
			ChangeState(State::Hover);
		}
		break;
	}
}


void Pink::DrawAttack(const Camera& camera) {
	//cameraは攻撃エフェクトをworld座標から
	//Screen座標へ変換するときに使用する
	(void)camera;

	switch (state_.current) {
	case State::Hover:
		Novice::ScreenPrintf(10, 80, "Boss State : Hover");
		break;
	case State::Magma:
		Novice::ScreenPrintf(10, 80, "Boss State : Magma");
		break;
	case State::BeamWarning:
	{
		Novice::ScreenPrintf(10, 80, "Boss State ; Beam Warning");

		Vector2 beamStart = GetPosition();
		Vector2 beamEnd = {
			beamStart.x + beam_.direction.x * beam_.length,
			beamStart.y + beam_.direction.y * beam_.length
		};

		Vector2 startScreen =
			camera.WorldToScreen(beamStart);
		Vector2 endScreen =
			camera.WorldToScreen(beamEnd);

		//予告円
		int circleRadius = 35 + state_.timer % 20;

		Novice::DrawEllipse(
			static_cast<int>(startScreen.x),
			static_cast<int>(startScreen.y),
			circleRadius,
			circleRadius,
			0.0f,
			0xFF4444FF,
			kFillModeWireFrame
		);

		Novice::DrawLine(
			static_cast<int>(startScreen.x),
			static_cast<int>(startScreen.y),
			static_cast<int>(endScreen.x),
			static_cast<int>(endScreen.y),
			0xFF4444AA
		);
		break;
	}
	case State::Beam:
	{
		Novice::ScreenPrintf(10, 80, "Boss State : Beam");

		Vector2 beamStart = GetPosition();

		Vector2 beamEnd = {
			beamStart.x + beam_.direction.x * beam_.length,
			beamStart.y + beam_.direction.y * beam_.length
		};

		Vector2 startScreen =
			camera.WorldToScreen(beamStart);
		Vector2 endScreen =
			camera.WorldToScreen(beamEnd);

		//ビームに対して垂直な方向
		Vector2 perpendicular = {
			-beam_.direction.y,
			beam_.direction.x
		};
		//複数の線を並べて太いビームにする
		for (int offset = -beam_.halfWidth;
			offset <= beam_.halfWidth;
			offset++) {
			float offsetX =
				perpendicular.x * static_cast<float>(offset);
			float offsetY =
				perpendicular.y * static_cast<float>(offset);

			Novice::DrawLine(
				static_cast<int>(startScreen.x + offsetX),
				static_cast<int>(startScreen.y + offsetY),
				static_cast<int>(endScreen.x + offsetX),
				static_cast<int>(endScreen.y + offsetY),
				0xFF66CCFF
			);
		}
		//中心を白くする
		Novice::DrawLine(
			static_cast<int>(startScreen.x),
			static_cast<int>(startScreen.y),
			static_cast<int>(endScreen.x),
			static_cast<int>(endScreen.y),
			WHITE
		);

		break;
	}
	case State::FlameThrower:
		Novice::ScreenPrintf(10, 80, "Boss State : Flame Thrower");
		break;
	case State::Strike:
		Novice::ScreenPrintf(10, 80, "Boss State : Strike");
		break;
	case State::Stunned:
		Novice::ScreenPrintf(10, 80, "Boss State : stunned");
		break;
	}
}

void Pink::DrawBarrier(const Camera& camera) {
	if (!barrier_.isActive) {
		return;
	}

	Vector2 bossScreenPosition =
		camera.WorldToScreen(GetPosition());

	int pulse = barrier_.animationTimer % 30;
	if (pulse > 15) {
		pulse = 30 - pulse;
	}

	const int baseRadius = 55 + pulse / 3;

	for (int i = 0; i < barrier_.hp; i++) {
		const int radius = baseRadius + i * 12;

		Novice::DrawEllipse(
			static_cast<int>(bossScreenPosition.x),
			static_cast<int>(bossScreenPosition.y),
			radius,
			radius,
			0.0f,
			0xFF66FFFF,
			kFillModeWireFrame
		);
	}

	Novice::ScreenPrintf(
		10,
		100,
		"Barrier HP : %d",
		barrier_.hp
	);
}

bool Pink::IsBeamHit(
	Vector2 playerPosition,
	float playerRadius
) const
{
	if (state_.current != State::Beam) {
		return false;
	}

	Vector2 beamStart = GetPosition();

	Vector2 beamEnd = {
		beamStart.x + beam_.direction.x * beam_.length,
		beamStart.y + beam_.direction.y * beam_.length
	};

	Vector2 beamVector = {
		beamEnd.x - beamStart.x,
		beamEnd.y - beamStart.y
	};

	Vector2 playerVector = {
		playerPosition.x - beamStart.x,
		playerPosition.y - beamStart.y
	};

	float beamLengthSquared =
		beamVector.x * beamVector.x +
		beamVector.y * beamVector.y;

	if (beamLengthSquared <= 0.001f) {
		return false;
	}

	float t =
		(playerVector.x * beamVector.x +
			playerVector.y * beamVector.y) /
		beamLengthSquared;

	if (t < 0.0f) {
		t = 0.0f;
	} else if (t > 1.0f) {
		t = 1.0f;
	}

	Vector2 nearestPosition = {
		beamStart.x + beamVector.x * t,
		beamStart.y + beamVector.y * t
	};

	float distanceX =
		playerPosition.x - nearestPosition.x;

	float distanceY =
		playerPosition.y - nearestPosition.y;

	float hitRadius =
		static_cast<float>(beam_.halfWidth) +
		playerRadius;

	return distanceX * distanceX +
		distanceY * distanceY
		<= hitRadius * hitRadius;
}

void Pink::SelectMoveTarget() {
	const int left =
		4 * kTileSize;

	const int rigth =
		(kBossMapWidth - 4) * kTileSize;

	const int top =
		3 * kTileSize;

	//ボスはマップ上半分を移動	
	const int bottom =
		(kBossMapHeight / 2) * kTileSize;

	moveTarget_.x = static_cast<float>(left + rand() % (rigth - left + 1));
	moveTarget_.y = static_cast<float>(top + rand() % (bottom - top + 1));

	//次の目的地を選びなおすまで2~4秒
	moveTimer_ =
		120 + rand() % 121;
}
