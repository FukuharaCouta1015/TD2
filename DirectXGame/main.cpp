#include <Windows.h>
#include "kamataEngine.h"
#include "SceneManager.h"
#include <cstdio> // <--- この行を追加

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
    // デバッグ用のコンソールウィンドウを作成 (DEBUGビルドでのみ有効にするのが一般的)
#ifdef _DEBUG
    AllocConsole(); // コンソールをアタッチ
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout); // stdout をコンソールにリダイレクト
#endif

	// KamataEngineの初期化
	KamataEngine::Initialize(L"TD2_2166");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();	

	// SceneManagerの初期化
	SceneManager sceneManager;
	sceneManager.Initialize();

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	//メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		imguiManager->Begin();

		// sceneManagerの更新
		sceneManager.ChangeScene();
		sceneManager.Update();

		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		sceneManager.Draw();

		imguiManager->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	//エンジンの終了処理
	KamataEngine::Finalize();
#ifdef _DEBUG
    // プログラム終了時にコンソールを解放 (オプション)
    FreeConsole();
#endif
	return 0;
}
