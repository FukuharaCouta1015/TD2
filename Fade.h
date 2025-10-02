#pragma once
#include "kamataEngine.h"

class Fade {

	
public:

	//フェードの状態
	enum class State {
		None,
		FadeIn,
		FadeOut,

	};

	// デストラクタ
	
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	void Start(State status, float duration); 

	void Stop();

	bool IsFinished() const;

	private:
		//テクスチャハンドル
	    uint32_t textureHandle_ = 0; 
		//スプライト
	    KamataEngine::Sprite* sprite_ = nullptr;
	
		// フェードの状態
	    State status_ = State::None;

		float duration_ = 0.0f; // フェードの持続時間
	    float counter_ = 0.0f;    // タイマー
};
