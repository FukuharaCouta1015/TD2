#pragma once
#include "GameScene.h"
#include "TitleScene.h"

enum class SceneType {
	kUnknown = 0,
	kTitle,
	kGame,
};

class SceneManager {
public:
	SceneManager();
	~SceneManager();

	void Initialize();
	void Update();
	void Draw();
	void ChangeScene();

private:
	SceneType currentScene_;
	GameScene* gameScene_;
	TitleScene* titleScene_;
};