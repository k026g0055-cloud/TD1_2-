#include <Novice.h>
#include <Xinput.h>

#include "InputManager.h"
#include "Player.h"
#include "Boss.h"
#include "GameConfig.h"
#include "Map.h"
#include "Camera.h"
#include "BLUE_BOSS.h"
#include "QTESystem.h"

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

	QTESystem qte;
	qte.Initialize();

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
			//デバック用
			if (input.IsButtonTrigger(XINPUT_GAMEPAD_RIGHT_THUMB))
			{
				currentSceen = BOSS;
			}

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
				player.SetPosition({ 640.0f,620.0f });
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
			map.Debug(camera);
			player.Draw(camera);

			break;
		}

		case BOSS:
		{
			Novice::ScreenPrintf(10, 10, "boss");

			bool qteStarted = false;

			// QTE開始処理
			if (!qte.IsPlaying())
			{
				// 初心者QTE
				if (input.IsKeyTrigger(DIK_1) ||
					input.IsButtonTrigger(XINPUT_GAMEPAD_DPAD_LEFT))
				{
					qte.Start(QTE_BEGINNER);
					qteStarted = true;
				}
				// 上級者QTE
				else if (input.IsKeyTrigger(DIK_2) ||
					input.IsButtonTrigger(XINPUT_GAMEPAD_DPAD_RIGHT))
				{
					qte.Start(QTE_ADVANCED);
					qteStarted = true;
				}
			}

			// QTE更新
			if (qte.IsPlaying())
			{
				if (!qteStarted)
				{
					qte.Update(input);
				}
			}

			// 移動処理
			if (!qte.IsPlaying() ||
				qte.GetMode() == QTE_BEGINNER)
			{
				player.Update(input, map);
			}

			// カメラ更新
			camera.Update(player.GetPosition());

			// ボス更新
			boss.Update();

			// 描画処理
			map.Debug(camera);
			boss.Draw(camera);
			player.Draw(camera);

			// QTE操作説明
			Novice::ScreenPrintf(
				10, 200,
				"DPAD LEFT : BEGINNER"
			);

			Novice::ScreenPrintf(
				10, 220,
				"DPAD RIGHT : ADVANCED"
			);

			// QTEは最後に描画
			qte.Draw();

			break;
		}

		case BLUE_BOSS:
		{
			Novice::ScreenPrintf(10, 10, "blue boss");

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

			}

			map.Debug(camera);
			blue_boss.Draw();
			boss.Draw(camera);
			player.Draw(camera);

			// QTEは最後に描画
			qte.Draw();

			Novice::ScreenPrintf(
				10, 200,
				"DPAD LEFT : BEGINNER"
			);

			Novice::ScreenPrintf(
				10, 220,
				"DPAD RIGHT : ADVANCED"
			);

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
