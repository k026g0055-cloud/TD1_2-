#include <Novice.h>
enum Sceen
{
	TITLE,
	PLAY
};


const char kWindowTitle[] = "LC1C_09_シミズ_イツキ";
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	HWND gameWindow = GetActiveWindow();

	LONG_PTR windowStyle = GetWindowLongPtr(gameWindow, GWL_STYLE);
	RECT windowRect = {};
	GetWindowRect(gameWindow, &windowRect);

	bool isFullScreen = false;


	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	int currentSceen = TITLE;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
		{


			if (!isFullScreen)
			{
				GetWindowRect(gameWindow, &windowRect);

				SetWindowLongPtr(
					gameWindow,
					GWL_STYLE,
					WS_POPUP | WS_VISIBLE
				);

				HMONITOR monitor = MonitorFromWindow(
					gameWindow,
					MONITOR_DEFAULTTONEAREST
				);

				MONITORINFO monitorInfo = {};
				monitorInfo.cbSize = sizeof(MONITORINFO);
				GetMonitorInfo(monitor, &monitorInfo);

				SetWindowPos(
					gameWindow,
					HWND_TOP,
					monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.top,
					monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
					SWP_FRAMECHANGED | SWP_SHOWWINDOW
				);

				isFullScreen = true;
			}
			else
			{
				SetWindowLongPtr(
					gameWindow,
					GWL_STYLE,
					windowStyle
				);

				SetWindowPos(
					gameWindow,
					HWND_NOTOPMOST,
					windowRect.left,
					windowRect.top,
					windowRect.right - windowRect.left,
					windowRect.bottom - windowRect.top,
					SWP_FRAMECHANGED | SWP_SHOWWINDOW
				);

				isFullScreen = false;
			}
		}


		switch (currentSceen)
		{
		case TITLE:
		{
			//pull request test
			

			break;

		}
		case PLAY:
		{


			break;
		}

		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		///
		/// ↑描画処理ここまで
		///

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
