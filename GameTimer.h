#pragma once

class GameTimer
{
public:
	//制限時間を設定して、0フレーム目から数え始める
	void Start(int frames) {
		duration_ = frames;
		elapsed_ = 0;
	}

	//経過時間だけ数えるときに使う
	void Reset() {
		Start(0);
	}

	//経過時間を1フレーム進める
	void Update() {
		++elapsed_;
	}

	//タイマーを開始してから経過したフレーム数を返す
	int GetElapsed() const {
		return elapsed_;
	}

	//残りのフレーム数を返し、制限時間に達したら0を返す
	int GetRemaining() const {
		return elapsed_ < duration_ ? duration_ - elapsed_ : 0;
	}

	//残り時間が0ならtrueを返す
	bool IsFinished() const {
		return GetRemaining() == 0;
	}
private:
	int duration_ = 0;
	int elapsed_ = 0;
};