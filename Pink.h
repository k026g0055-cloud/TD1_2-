#pragma once
#include "Boss.h"

class Camera;

class Pink : public Boss
{
public:
	Pink();

	void Update();
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

	void ChangeState(State nextState);
	void UpdateState();
	void DrawAttack(const Camera& camera);

	StateData state_;
	BarrierData barrier_;
};
