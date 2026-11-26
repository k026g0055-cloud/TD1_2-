#pragma once
#include "Boss.h"
#include "GameTimer.h"

class Camera;

class Pink : public Boss
{
public:
	Pink();

	void Update(Vector2 playerPosition);
	void Draw(const Camera& camera);
	void DamageBarrier(int damage);
	bool IsBarrierActive() const;
	bool IsBarrierHit(Vector2 attackCenter, Vector2 attackHalfSize) const;
	bool IsBeamHit(
		Vector2 playerPosition,
		float playerRadius
	) const;
	bool IsMagmaHit(
		Vector2 playerPosition,
		Vector2 playerHalfSize
	) const;
	bool IsFlameHit(
		Vector2 playerPosition,
		Vector2 playerHalfSize
	) const;

private:
	enum class State
	{
		Hover,
		Magma,
		BeamWarning,
		Beam,
		FlameWarning,
		FlameThrower,
		Strike,
		Stunned
	};


	//状態を管理する構造体
	struct StateData {
		State current = State::Hover;
		int timer = 0;
	};

	//バリアを管理する構造体
	struct BarrierData {
		int hp = 3;
		bool isActive = false;
		int animationTimer = 0;
	};

	struct BeamData {
		Vector2 direction = { 0.0f,1.0f };
		float length = 2000.0f;
		int halfWidth = 12;
	};

	struct MagmaData {
		Vector2 position = {};
		Vector2 velocity = {};

		float landingY = 0.0f;
		float radius = 24.0f;
		float gravity = 0.35f;

		GameTimer remainTimer;

		bool isActive = false;
		bool isLanded = false;

	};

	struct FlameCircle {
		Vector2 position;
		int radius;
	};

	void ChangeState(State nextState);
	void ActivateBarrier();
	void DrawAttack(const Camera& camera);
	void DrawBarrier(const Camera& camera);
	void PrepareBeam(Vector2 playerPosition);
	void UpdateState(Vector2 playerPosition);
	void UpdateMovement();
	void SelectMoveTarget();
	void SpawnMagma(Vector2 playerPosition);
	void UpdateMagma();
	void DrawMagma(const Camera& camera);
	void DrawFlame(const Camera& camera, bool isWarning) const;

	static const int kMaxMagma = 30;
	static constexpr float kFlameRange = 240.0f;
	static constexpr float kFlameHalfAngle = 0.4f;

	Vector2 moveTarget_ = {};
	float moveSpeed_ = 2.0f;
	float flameAngle_ = 0.0f;
	int moveTimer_ = 0;
	int GetBarrierBaseRadius() const;

	FlameCircle GetFlameCircle(int row, int Iane) const;
	BeamData beam_;
	StateData state_;
	BarrierData barrier_;
	MagmaData magma_[kMaxMagma];
};
