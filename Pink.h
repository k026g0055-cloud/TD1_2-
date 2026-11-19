#pragma once
#include "Boss.h"

class Camera;

class Pink : public Boss
{
public:
	Pink();

	void Update(Vector2 playerPosition);
	void Draw(const Camera& camera);

private:
	enum class State
	{
		Hover,
		Magma,
		BeamWarning,
		Beam,
		FlameThrower,
		Strike,
		Barrier,
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
	};

	struct BeamDate {
		Vector2 direction = { 0.0f,1.0f };
		float length = 2000.0f;
		int halfWidth = 12;
	};

	void ChangeState(State nextState);
	void DrawAttack(const Camera& camera);
	void PrepareBeam(Vector2 playerPosition);
	void UpdateState(Vector2 playerPosition);

	BeamDate beam_;
	StateData state_;
	BarrierData barrier_;
};
