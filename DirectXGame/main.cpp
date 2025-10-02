#include <Windows.h>
#include "kamataEngine.h"
#include "GameScene.h"
#include "TitleScene.h"
using namespace KamataEngine;

GameScene* gameScene = nullptr; // ゲームシーンのインスタンス
TitleScene* titleScene = nullptr; // タイトルシーンのインスタンス

enum class Scene {
	kUnknown = 0,
	kTitle, // タイトルシーン
	kGame,  // ゲームシーン

};

Scene scene = Scene::kUnknown; // 現在のシーン

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
		//シーン変更
			scene = Scene::kGame;

			delete titleScene;
			titleScene = nullptr; // タイトルシーンのインスタンスを解放

			gameScene = new GameScene(); // ゲームシーンのインスタンスを生成
			gameScene->Initialize();     // ゲームシーンの初期化

		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			scene = Scene::kTitle; // タイトルシーンに戻る	
			delete gameScene;      // ゲームシーンのインスタンスを解放
			gameScene = nullptr;   // ゲームシーンのインスタンスをnullptrに設定

			titleScene = new TitleScene(); // タイトルシーンのインスタンスを生成
			titleScene->Initialize();      // タイトルシーンの初期化

		}
			
		break;
	}
}

void UpdateScene() {
	switch (scene) { 
		case Scene::kTitle:
		titleScene->Update(); // タイトルシーンの更新
		    break;
	    case Scene::kGame:
		    gameScene->Update(); // ゲームシーンの更新
		    break;
	}
}

// シーンの更新
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw(); // タイトルシーンの描画
		break;
	case Scene::kGame:
		gameScene->Draw(); // ゲームシーンの描画
		break;
	}
}


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// KamataEngineの初期化
	KamataEngine::Initialize(L" LE2D_19_フクハラ_コウタ");

	scene = Scene::kTitle; // 初期シーンをタイトルに設定

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();	

	// ゲームシーンのインスタンスを生成
 gameScene = new GameScene();
	// ゲームシーンの初期化
	gameScene->Initialize();


	titleScene = new TitleScene;
	titleScene->Initialize();

	//メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		
		ChangeScene();
		//titleScene->Update();
		UpdateScene();
		
		//gameScene->Update();

		// 描画開始
		dxCommon->PreDraw();
		// ここに描画処理を記述

	//	gameScene->Draw();
		//titleScene->Draw();
		DrawScene();

		// 描画終了
		dxCommon->PostDraw();
	}
	// ゲームシーンの解放
	delete gameScene;
	delete titleScene;
	gameScene = nullptr;

	//エンジンの終了処理
	KamataEngine::Finalize();
	return 0;
}
