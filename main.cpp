#include <Novice.h>
#include "InputManager.h"
#include "Player.h"

enum Sceen
{
	TITLE,
	PLAY
};

const char kWindowTitle[] = "LC1C_09_シミズ_イツキ";
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	bool isFullScreen = false;

	// 入力管理
	InputManager input;
	input.Initialize();

	// プレイヤー
	Player player;
	player.Initialize();

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	int currentSceen = TITLE;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		input.Update();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		if (keys[DIK_F11] && !preKeys[DIK_F11])
		{
			isFullScreen = !isFullScreen;

			if (isFullScreen)
			{
				Novice::SetWindowMode(kFullscreen);
			}
			else
			{
				Novice::SetWindowMode(kWindowed);
			}
		}

		switch (currentSceen)
		{
		case TITLE:
		{
			Novice::ScreenPrintf(10, 10, "title");
			Novice::ScreenPrintf(10, 25, "space");
			
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
			{
				currentSceen = PLAY;
			}
			break;

		}
		case PLAY:
		{
			player.Update(input);
			player.Draw();

			break;
		}

		}

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
