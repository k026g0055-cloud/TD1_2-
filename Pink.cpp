#include "Pink.h"
#include "Camera.h"

#include <Novice.h>
#include <cstdlib>

Pink::Pink() {
	state_.current = State::Hover;
	state_.timer = 0;

	barrier_.hp = 3;
	barrier_.isActive = false;
}

void Pink::Update() {
	//共通処理：ワープと全方位弾
	Boss::Update();
	UpdateState();
}

void Pink::Draw(const Camera& camera) {
	Boss::Draw(camera);
	DrawAttack(camera);
}

void Pink::ChangeState(State nextState) {
	state_.current = nextState;
	state_.timer = 0;
	if (nextState == State::Barrier) {
		barrier_.hp = 3;
		barrier_.isActive = true;
	}
	if (nextState == State::Stunned) {
		barrier_.isActive = false;
	}
}

void Pink::UpdateState() {
	state_.timer++;
	switch (state_.current) {
	case State::Hover:
		if (state_.timer >= 120) {
			int attackNumber = rand() % 5;
			switch (attackNumber)
			{
			case 0:
				ChangeState(State::Magma);
				break;
			case 1:
				ChangeState(State::BeamWarning);
				break;
			case 2:
				ChangeState(State::FlameThrower);
				break;
			case 3:
				ChangeState(State::Strike);
				break;
			case4:
				ChangeState(State::Barrier);
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
			ChangeState(State::Hover)
		}
	}

}