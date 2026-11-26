#include <Novice.h>
#include <Xinput.h>
#include <cstdlib>
#include <ctime>

#include "InputManager.h"
#include "Player.h"
#include "Boss.h"
#include "GameConfig.h"
#include "Map.h"
#include "Camera.h"
#include "Pink.h"
#include "BLUE_BOSS.h"
#include "QTESystem.h"

#pragma comment(lib, "Xinput.lib")

enum Sceen
{
	TITLE,
	PLAY,
	BOSS,
	PINK,
	BLUE_BOSS,
	MENU
};

const char kWindowTitle[] = "LC1C_09_シミズ_イツキ";

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	bool isFullScreen = false;

	srand(static_cast<unsigned int>(time(nullptr)));

	// 入力管理
	InputManager input;
	input.Initialize();

	// プレイヤー
	Player player;
	player.Initialize();

	// ボス
	Boss boss;
	Pink pink;
	Blue_Boss blue_boss;

	// マップ
	static Map map;
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
			(currentSceen == PLAY || currentSceen == BOSS || currentSceen == PINK)) {
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
				player.Initialize();
				camera.Initialize(
					static_cast<float>(map.GetWidth()),
					static_cast<float>(map.GetHeight())
				);
				currentSceen = PLAY;
			}
			break;

		}
		case PLAY:
		{
			//デバック用
			if (input.IsButtonTrigger(XINPUT_GAMEPAD_RIGHT_THUMB) ||
				input.IsKeyTrigger(DIK_1))
			{
				currentSceen = BOSS;
			}

			Novice::ScreenPrintf(10, 10, "play");
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
				camera.Initialize(
					static_cast<float>(kBossMapWidth * kTileSize),
					static_cast<float>(kBossMapHeight * kTileSize)
				);

				// ボスマップの中央下へ配置
				player.SetPosition({
					kBossMapWidth * kTileSize / 2.0f,
					kBossMapHeight * kTileSize - 100.0f
					});
				pink = Pink{};
				currentSceen = PINK;
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

			qte.Debug();

			Vector2 PlayerPosition = player.GetPosition();
			int playerTileX = static_cast<int>(PlayerPosition.x) / kTileSize;
			int playerTileY = static_cast<int>(PlayerPosition.y) / kTileSize;

			if (playerTileX >= kMapWidth - 4 &&
				playerTileY <= 3) {
				player.SetPosition({ 640.0f,620.0f });
				currentSceen = BOSS;
				break;
			}
			bool qteStarted = false;

			// QTE開始
			if (!qte.IsPlaying())
			{
				if (input.IsKeyTrigger(DIK_1) ||
					input.IsButtonTrigger(XINPUT_GAMEPAD_DPAD_LEFT))
				{
					qte.Start(QTE_BEGINNER);
					qteStarted = true;
				}
				else if (input.IsKeyTrigger(DIK_2) ||
					input.IsButtonTrigger(XINPUT_GAMEPAD_DPAD_RIGHT))
				{
					qte.Start(QTE_ADVANCED);
					qteStarted = true;
				}
			}

			// QTE更新
			if (qte.IsPlaying() && !qteStarted)
			{
				qte.Update(input);
			}

			// プレイヤー更新
			if (!qte.IsPlaying())
			{
				player.Update(input, map);
			}
			else if (qte.GetMode() == QTE_BEGINNER)
			{
				player.UpdateMove(input, map);
			}

			// ボス専用フィールドから出ないようにする
			player.ClampToArea(
				static_cast<float>(kBossMapWidth * kTileSize),
				static_cast<float>(kBossMapHeight * kTileSize)
			);

			// カメラ更新
			camera.Update(player.GetPosition());

			// ボス更新
			boss.Update();

			// 描画
			map.DebugBoss(camera);
			boss.Draw(camera);
			player.Draw(camera);

			// 操作説明
			Novice::ScreenPrintf(
				10, 200,
				"DPAD LEFT : BEGINNER"
			);

			Novice::ScreenPrintf(
				10, 220,
				"DPAD RIGHT : ADVANCED"
			);

			// QTEは一番手前に描画
			qte.Draw();

			break;
		}

		case BLUE_BOSS:
		{
			Novice::ScreenPrintf(10, 10, "blue boss");

			player.Update(input, map);
			player.ClampToArea(
				static_cast<float>(kBossMapWidth * kTileSize),
				static_cast<float>(kBossMapHeight * kTileSize)
			);
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
		case PINK:
		{
			Novice::ScreenPrintf(10, 10, "PINK");

			player.Update(input, map);
			player.ClampToArea(
		
				static_cast<float>(kBossMapWidth * kTileSize),
				static_cast<float>(kBossMapHeight * kTileSize)
			);
			camera.Update(player.GetPosition());
			pink.Update(player.GetPosition());

			if (pink.IsBeamHit(
				player.GetPosition(),
				24.0f)) {
				Novice::ScreenPrintf(
					500, 100, "Beam HIT"
				);
			}
			if (pink.IsMagmaHit(player.GetPosition(), { 24.0f,24.0f })) {
				Novice::ScreenPrintf(
					500, 100, "Magma Hit"
				);
			}

			if (pink.IsFlameHit(player.GetPosition(), { 24.0f,24.0f })) {
				Novice::ScreenPrintf(
					500, 100, "Flame Hit"
				);
			}

			Vector2 PlayerPosition = player.GetPosition();
			int playerTileX = static_cast<int>(PlayerPosition.x) / kTileSize;
			int playerTileY = static_cast<int>(PlayerPosition.y) / kTileSize;

			if (playerTileX >= kMapWidth - 4 &&
				playerTileY <= 3) {
				player.SetPosition({ 640.0f,620.0f });
				currentSceen = PINK;
				break;

			}

			map.DebugBoss(camera);
			pink.Draw(camera);
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
