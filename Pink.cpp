#include "Pink.h"
#include "Camera.h"

#include <Novice.h>
#include <algorithm>
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

	UpdateState(playerPosition);
	UpdateMovement();
	UpdateMagma();

	if (barrier_.isActive) {
		barrier_.animationTimer++;
	}

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
	DrawMagma(camera);
}

void Pink::SpawnMagma(Vector2 playerPosition) {
	for (int i = 0;i < kMaxMagma;i++) {
		if (magma_[i].isActive) {
			continue;
		}
		float randomX =
			static_cast<float>(rand() % 321 - 160);

		float randomY =
			static_cast<float>(rand() % 321 - 160);

		Vector2 targetPosition = {
			playerPosition.x + randomX,
			playerPosition.y + randomY
		};

		const float margin = 48.0f;

		targetPosition.x = std::clamp(
			targetPosition.x,
			margin,
			static_cast<float>(
				kBossMapWidth * kTileSize
				) - margin
		);

		targetPosition.y = std::clamp(
			targetPosition.y,
			margin,
			static_cast<float>(
				kBossMapHeight * kTileSize
				) - margin
		);

		magma_[i].startPosition = GetPosition();
		magma_[i].targetPosition = targetPosition;
		magma_[i].position = GetPosition();

		magma_[i].radius = 24.0f;
		magma_[i].flightTimer = 0;
		magma_[i].flightTime = 60;
		magma_[i].remainTimer = 180;

		magma_[i].isActive = true;
		magma_[i].islended = false;

		break;
	}
}

void Pink::UpdateMovement()
{
	// ビーム・打撃・スタン中は移動しない
	if (state_.current == State::BeamWarning ||
		state_.current == State::Beam ||
		state_.current == State::Strike ||
		state_.current == State::Stunned)
	{
		return;
	}

	moveTimer_--;

	Vector2 position = GetPosition();

	Vector2 difference = {
		moveTarget_.x - position.x,
		moveTarget_.y - position.y
	};

	float distance = std::sqrt(
		difference.x * difference.x +
		difference.y * difference.y
	);

	// 目的地へ到着、または移動時間切れ
	if (distance <= moveSpeed_ ||
		moveTimer_ <= 0)
	{
		SelectMoveTarget();
		return;
	}

	Vector2 velocity = {
		difference.x / distance * moveSpeed_,
		difference.y / distance * moveSpeed_
	};

	position.x += velocity.x;
	position.y += velocity.y;

	SetPosition(position);
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
		if (state_.timer == 1 ||
			(state_.timer <= 80 &&
				state_.timer % 20 == 0)) {
			SpawnMagma(playerPosition);
		}
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

void Pink::UpdateMagma() {
	for (int i = 0;i < kMaxMagma;i++) {
		if (!magma_[i].isActive) {
			continue;
		}

		if (!magma_[i].islended) {
			magma_[i].flightTimer++;

			float t =
				static_cast<float>(
					magma_[i].flightTimer
					) /
				static_cast<float>(
					magma_[i].flightTimer
					);

			if (t > 1.0f) {
				t = 1.0f;
			}

			magma_[i].position.x =
				magma_[i].startPosition.x +
				(
					magma_[i].targetPosition.x -
					magma_[i].startPosition.x
					) * t;
			if (magma_[i].flightTimer >=
				magma_[i].flightTime) {
				magma_[i].position =
					magma_[i].targetPosition;
				magma_[i].islended = true;
			}
		} else {
			//着弾後3秒間残る処理
			magma_[i].remainTimer--;

			if (magma_[i].remainTimer <= 0) {
				magma_[i].isActive = false;
				magma_[i].islended = false;
			}
		}
	}
}

void Pink::DrawMagma(const Camera& camera) {
	const float kPi = 3.14f;

	for (int i = 0;i < kMaxMagma;i++) {
		if (!magma_[i].isActive) {
			continue;
		}

		Vector2 screenPosition =
			camera.WorldToScreen(
				magma_[i].position
			);

		if (!magma_[i].islended) {
			float t =
				static_cast<float>(
					magma_[i].flightTimer
					) /
				static_cast<float>(
					magma_[i].flightTimer
					);
			if (t > 1.0f) {
				t = 1.0f;
			}

			//放射物の高さ
			float arcHeight =
				std::sin(t * kPi) * 100.0f;

			screenPosition.y -= arcHeight;

			//飛んでいるマグマ
			Novice::DrawEllipse(
				static_cast<int>(screenPosition.x),
				static_cast<int>(screenPosition.y),
				static_cast<int>(magma_[i].radius),
				static_cast<int>(magma_[i].radius),
				0.0f,
				0xFF4400FF,
				kFillModeSolid
			);

			//中心部分
			Novice::DrawEllipse(
				static_cast<int>(screenPosition.x),
				static_cast<int>(screenPosition.y),
				static_cast<int>(
					magma_[i].radius / 2.0f
					),
				static_cast<int>(
					magma_[i].radius / 2.0f
					),
				0.0f,
				0xFFFF00FF,
				kFillModeSolid
			);
		} else {
			//地面に残ったマグマ
			int pulse =
				magma_[i].remainTimer % 20;

			int radius =
				static_cast<int>(
					magma_[i].radius
					) + pulse / 4;
			Novice::DrawEllipse(
				static_cast<int>(screenPosition.x),
				static_cast<int>(screenPosition.y),
				radius,
				radius / 2,
				0.0f,
				0xFF3300FF,
				kFillModeSolid
			);

			Novice::DrawEllipse(
				static_cast<int>(screenPosition.x),
				static_cast<int>(screenPosition.y),
				radius / 2,
				radius / 4,
				0.0f,
				0xFFAA00FF,
				kFillModeSolid
			);
		}
	}
}