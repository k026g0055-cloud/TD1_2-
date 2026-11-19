#include <Novice.h>
#include <Xinput.h>

#include "InputManager.h"
#include "Player.h"
#include "Boss.h"
#include "GameConfig.h"
#include "Map.h"
#include "Camera.h"
#include "BLUE_BOSS.h"

#pragma comment(lib, "Xinput.lib")

enum Sceen
{
	TITLE,
	PLAY,
	BOSS,
	BLUE_BOSS,
	MENU
};

const char kWindowTitle[] = "LC1C_09_シミズ_イツキ";

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	bool isFullScreen = false;

	// 入力管理
	InputManager input;
	input.Initialize();

	// プレイヤー
	Player player;
	player.Initialize();

	// ボス
	Boss boss;
	Blue_Boss blue_boss;

	// マップ
	Map map;
	map.Initialize();

	// カメラ
	Camera camera;
	camera.Initialize(
		static_cast<float>(map.GetWidth()),
		static_cast<float>(map.GetHeight())
	);

	int currentSceen = TITLE;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		//毎フレーム入力を受け取る
		input.Update();
		input.DrawDebug();

		//フルスクリーン
		if (input.IsKeyTrigger(DIK_F11) ||
			input.IsButtonTrigger(XINPUT_GAMEPAD_START))
		{
			isFullScreen = !isFullScreen;

			if (isFullScreen)
			{
				Novice::SetWindowMode(kFullscreen);
			} else
			{
				Novice::SetWindowMode(kWindowed);
			}
		}

		if ((input.IsKeyTrigger(DIK_ESCAPE) || input.IsButtonTrigger(XINPUT_GAMEPAD_BACK)) &&
			(currentSceen == PLAY || currentSceen == BOSS)) {
			currentSceen = MENU;
		}
		switch (currentSceen)
		{
		case TITLE:
		{
			Novice::ScreenPrintf(10, 10, "title");
			Novice::ScreenPrintf(10, 25, "space or A");

			//SPACEかAボタンを押したら
			if (input.IsKeyTrigger(DIK_SPACE) ||
				input.IsButtonTrigger(XINPUT_GAMEPAD_A))
			{
				currentSceen = PLAY;
			}
			break;

		}
		case PLAY:
		{
			Novice::ScreenPrintf(10, 10, "play");
			player.Update(input,map);
			player.Update(input, map);
			Vector2 PlayerPosition = player.GetPosition();

			int playerTileX = static_cast<int>(PlayerPosition.x) / kTileSize;
			int playerTileY = static_cast<int>(PlayerPosition.y) / kTileSize;

			Novice::ScreenPrintf(
				10, 50,
				"Tile X:%d Y:%d",
				playerTileX, playerTileY
			);

			if (playerTileX >= kMapWidth - 4 &&
				playerTileY <= 3) {
				currentSceen = BOSS;
				break;
			}

			if (playerTileX >= kMapWidth - 4 &&
				playerTileY <= kMapHeight - 4) {
				player.SetPosition({ 640.0f,620.0f });
				currentSceen = BLUE_BOSS;
				break;

			}

			player.Debug();
			camera.Update(player.GetPosition());
			player.Draw(camera);

			break;
		}
		case BOSS:
		{
			Novice::ScreenPrintf(10, 10, "boss");

			player.Update(input, map);
			camera.Update(player.GetPosition());
			blue_boss.Update();

			Vector2 PlayerPosition = player.GetPosition();
			int playerTileX = static_cast<int>(PlayerPosition.x) / kTileSize;
			int playerTileY = static_cast<int>(PlayerPosition.y) / kTileSize;

			if (playerTileX >= kMapWidth - 4 &&
				playerTileY <= kMapHeight - 4) {
				player.SetPosition({ 640.0f,620.0f });
				currentSceen = BLUE_BOSS;
				break;


			map.Debug(camera);
			boss.Draw(camera);
			player.Draw(camera);

			break;
		}
		case MENU:
		{
			Novice::ScreenPrintf(540, 300, "MENU");
			Novice::ScreenPrintf(500, 350, "> Return to Title");
			Novice::ScreenPrintf(480, 390, "SPACE or A : Select");

			if (input.IsKeyTrigger(DIK_SPACE) ||
				input.IsButtonTrigger(XINPUT_GAMEPAD_A)) {
				currentSceen = TITLE;
			}
			break;
		}
		}

		// フレームの終了
		Novice::EndFrame();

		
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
