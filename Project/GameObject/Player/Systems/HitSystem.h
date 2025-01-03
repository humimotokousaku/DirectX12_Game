#pragma once
#include "MathStructs.h"
#include "Animation.h"
#include "Shake.h"
#include "Sprite.h"
#include "Input.h"
#include <array>

/// <summary>
/// 被弾したときの処理をまとめるクラス
/// </summary>
class HitSystem {
public:
	HitSystem() = default;
	~HitSystem() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	/// <summary>
	/// 被弾時の更新処理
	/// </summary>
	void HitUpdate();

public:
#pragma region Setter
	/// <summary>
	/// 演出終了時間を設定
	/// </summary>
	/// <param name="endFrame">演出終了時間</param>
	void SetEndFrame(float endFrame) { 
		endFrame_ = endFrame;
		currentFrame_ = endFrame_;
	}
	/// <summary>
	/// 被弾演出開始
	/// </summary>
	void Start() { isHit_ = true; }
#pragma endregion

#pragma region Getter
	/// <summary>
	/// 被弾時の自機の回転角
	/// </summary>
	/// <returns></returns>
	const Vector3& GetHitRotate() { return hitRotate_; }
	/// <summary>
	/// 被弾演出中か
	/// </summary>
	/// <returns></returns>
	const bool& GetIsHit() { return isHit_; }
#pragma endregion

private:
	// 入力
	Input* input_;

private:
	// 被弾時のフィルター
	std::unique_ptr<Sprite> hitSprite_;

	// 被弾時の自機のアニメーション
	Animation hitAnim_;
	// 被弾時のフィルターアニメーション
	Animation hitSpriteAnim_;

	// 被弾時の自機の角度
	Vector3 hitRotate_;

	// 演出終了時間
	float endFrame_;
	// 経過時間
	float currentFrame_;

	// 被弾演出中か
	bool isHit_ = false;
	bool isPreHit_ = false;
};