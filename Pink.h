#pragma once
#include "Boss.h"

class Camera;

class Pink : public Boss
{
public:
	Pink();

	void Update(Vector2 playerPosition);
	void Draw(const Camera& camera);
	void DamageBarrier(int damage);
	bool IsBarrierActive() const;
	bool IsBeamHit(
		Vector2 playerPosition,
		float playerRadius
	) const;

private:
	enum class State
	{
		Hover,
		Magma,
		BeamWarning,
		Beam,
		FlameThrower,
		Strike,
		Stunned
	};

	static const int kMaxMagma = 10;

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

	struct BeamDate {
		Vector2 direction = { 0.0f,1.0f };
		float length = 2000.0f;
		int halfWidth = 12;
	};

	struct MagmaData {
		Vector2 startPosition = {};
		Vector2 targetPosition = {};
		Vector2 position = {};

		float radius = 24.0f;

		int flightTimer = 0;
		int flightTime = 60;
		int remainTimer = 0;

		bool isActive = false;
		bool islended = false;
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

	Vector2 moveTarget_ = {};
	float moveSpeed_ = 2.0f;
	int moveTimer_ = 0;

	BeamDate beam_;
	StateData state_;
	BarrierData barrier_;
	MagmaData magma_[kMaxMagma];
};
