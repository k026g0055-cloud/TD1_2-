#pragma once
#include "Vector2.h"
#include "Camera.h"
#include "InputManager.h"
#include "Map.h"
#include "GameTimer.h"

class Player
{
public:
	void Initialize();
	void Update(const InputManager& input,const Map& map);
	void UpdateMove(const InputManager& input,const Map &map);
	void ClampToArea(float areaWidth, float areaHeight);
	Vector2 GetPosition() const;
	void SetPosition(Vector2 position);
	void Draw(const Camera& camera) const;
	void Debug();
	void UpdateAttackA();
	void UpdateAttackB();

private:

	enum State
	{
		NORMAL,
		ATTACK_A,
		ATTACK_B
	};

	State state_ = NORMAL;

	Vector2 position_ = {};
	Vector2 size_ = { 48.0f, 48.0f };
	Vector2 direction_ = { 0.0f, 1.0f };
	int hp_ = 5;

	GameTimer attackTimer_;
	int attackDamage_ = 0;
	bool isAttackActive = false;
	bool hasAttackHit_ = false;

	Vector2 attackPosition = {};
	Vector2 attackSize = {};

	int hp_ = 5;

	float speed_ = 5.0f;

};
