#pragma once
#include "MathStructs.h"
#include "GameTime.h"
#include <list>
#include <assert.h>
#include <functional>
#include <variant>

class Animation {
public: // 構造体
	// アニメーションを行うときのパラメータ
	struct AnimData {
		std::variant<Vector4*, Vector3*, Vector2*, float*, int*> target;	// アニメーションの対象
		float currentFrame;													// 現在のフレーム
		float startFrame;													// アニメーション開始フレーム
		float endFrame;														// アニメーション終了フレーム
		float t;															// 変化量
		std::variant<Vector4, Vector3, Vector2, float, int> start;			// 始めの値
		std::variant<Vector4, Vector3, Vector2, float, int> end;			// 終了時の値
		bool isActive;  													// アニメーションをしているか
		int id;																// アニメーションの番号
		std::function<float(float)> easeFunc;								// 使用するイージング関数
	};

public: // メンバ関数
	///
	/// Default Method
	/// 

	Animation();
	~Animation() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	//void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	//void Draw();

	///
	/// User Method
	/// 

	// 全てのアニメーションを削除
	void AllClearAnimation() { animData_.clear(); }

	// アニメーション情報をリセット
	void ResetData() {
		// 全てのデータを初期化
		for (std::list<AnimData>::iterator it = animData_.begin(); it != animData_.end(); ++it) {
			it->t = 0;
			it->currentFrame = 0;
			it->isActive = false;
		}
		isEnd_ = false;
		isStart_ = false;
	}

#pragma region Getter
	// アニメーションを始めるかを取得
	bool GetIsStart() { return isStart_; }
	// アニメーションが終了しているかを取得
	bool GetIsEnd() { return isEnd_; }
#pragma endregion

#pragma region Setter
	// アニメーションを始めるかを設定
	void SetIsStart(bool isStart) { 
		if (isStart_ != isStart) {
			for (std::list<AnimData>::iterator it = animData_.begin(); it != animData_.end(); ++it) {
				// 最初のアニメーションを起動
				if (it->id == 0) {
					it->isActive = isStart;
					break;
				}
			}
		}
		isStart_ = isStart;
	}

	/// <summary>
	/// アニメーションに必要なパラメータをリストに登録
	/// </summary>
	/// <param name="target">アニメーションする対象</param>
	/// <param name="start">最初のパラメータ</param>
	/// <param name="end">終了時のパラメータ</param>
	/// <param name="endFrame">アニメーション終了時間</param>
	/// <param name="easeFunc">使用するイージング関数</param>
	void SetAnimData(std::variant<Vector4*, Vector3*, Vector2*, float*, int*> target, std::variant<Vector4, Vector3, Vector2, float, int> start, std::variant<Vector4, Vector3, Vector2, float, int> end, float endFrame, std::function<float(float)> easeFunc);
	// アニメーションに必要なパラメータを先頭のリスト情報を上書き
	// 無い場合はリストに登録
	void SetFirstAnimData(std::variant<Vector4*, Vector3*, Vector2*, float*, int*> target, std::variant<Vector4, Vector3, Vector2, float, int> start, std::variant<Vector4, Vector3, Vector2, float, int> end, float endFrame, std::function<float(float)> easeFunc);
	/// <summary>
	/// 任意のアニメーション情報を上書き
	/// </summary>
	/// <param name="index">対象の要素番号</param>
	/// <param name="target">アニメーションする対象</param>
	/// <param name="start">最初のパラメータ</param>
	/// <param name="end">終了時のパラメータ</param>
	/// <param name="endFrame">アニメーション終了時間</param>
	/// <param name="easeFunc">使用するイージング関数</param>
	void AnimDataOverride(int index, std::variant<Vector4*, Vector3*, Vector2*, float*, int*> target, std::variant<Vector4, Vector3, Vector2, float, int> start, std::variant<Vector4, Vector3, Vector2, float, int> end, float endFrame, std::function<float(float)> easeFunc);
#pragma endregion

private:// エンジン機能
	// ゲームタイマー
	GameTimer* gameTimer_;

private: // メンバ変数
	std::list<AnimData> animData_;
	// アニメーションの順番
	int animId_;
	// アニメーションスタート
	bool isStart_;
	// アニメーションが終了しているか
	bool isEnd_;
};