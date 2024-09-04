#pragma once
#include "Sprite.h"
#include "PostEffectManager.h"
#include "Input.h"
#include "Animation.h"
#include <array>

// スコアクラス
// 注意点:スコアの加算をするときは1フレームだけ加算関数を呼び出すこと
class Score {
public:
	/// 
	/// Default Method
	/// 

	// シングルトン
	static Score* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	//void Draw();

	///
	/// User Method
	/// 

#pragma region Getter
	// スコアの取得
	int GetScore() { return score_; }
#pragma endregion

#pragma region Setter
	/// <summary>
	/// スコアの加算
	/// </summary>
	/// <param name="value">加算量</param>
	void AddScore(int value);
#pragma endregion

private:
	TextureManager* textureManager_;
	PostEffectManager* postEffectManager_;

	std::array<uint32_t, 10> numbersTexture_;

	// スコアのスプライト(サイズが桁数)
	std::array<std::unique_ptr<Sprite>, 7> numSprite_;

	int goalScore_;
	int score_;
	int tempScore_;
	std::array<int, 7> eachScore_;

	Animation scoreAnim_;
};