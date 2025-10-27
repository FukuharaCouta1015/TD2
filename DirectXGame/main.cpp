#include <Windows.h>
#include "kamataEngine.h"
#include "SceneManager.h"
using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// KamataEngineの初期化
	KamataEngine::Initialize(L"TD2_2166");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();	

	// SceneManagerの初期化
	SceneManager sceneManager;
	sceneManager.Initialize();

	//メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		// sceneManagerの更新
		sceneManager.ChangeScene();
		sceneManager.Update();

		// 描画開始
		dxCommon->PreDraw();

		sceneManager.Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	//エンジンの終了処理
	KamataEngine::Finalize();
	return 0;
}
