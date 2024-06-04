#pragma once
#include "MathStructs.h"
#include <list>
#include <assert.h>
#include <functional>

class Animation {
public: // 構造体
	// アニメーションを行うときのパラメータ
	struct AnimData {
		Vector3* target;       // アニメーションの対象
		uint32_t currentFrame; // 現在のフレーム
		uint32_t endFrame;	   // アニメーション終了フレーム
		float t;			   // 変化量
		Vector3 start;		   // 始めの値
		Vector3 end;		   // 終了時の値
		bool isActive;  	   // アニメーションをしているか
		const char* name;	   // アニメーションの名前
		uint32_t id;		   // アニメーションの番号
		std::function<float(float)> easeFunc; // 使用するイージング関数
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
	//void Fianlize();

	///
	/// User Method
	/// 

	// 特定のアニメーションを削除
	void ClearAnimation(AnimData& animData) {}
	// 全てのアニメーションを削除
	void AllClearAnimation() { animData_.clear(); }

	/// Getter
	// アニメーションを始めるかを取得
	bool GetIsStart() { return isStart_; }

	/// Setter
	// アニメーションを始めるかを設定
	void SetIsStart(bool isStart) { isStart_ = isStart; }

	/// <summary>
	///  アニメーションに必要なパラメータをリストに登録
	/// </summary>
	/// <param name="target">アニメーションする対象</param>
	/// <param name="start">最初のパラメータ</param>
	/// <param name="end">終了時のパラメータ</param>
	/// <param name="endFrame">アニメーション終了時間</param>
	/// <param name="name">アニメーションの名前</param>
	/// <param name="easeFunc">使用するイージング関数</param>
	void SetAnimData(Vector3* target, Vector3 start, Vector3 end, uint32_t endFrame, const char* name, std::function<float(float)> easeFunc);

private: // メンバ変数
	std::list<AnimData> animData_;
	// アニメーションの順番
	uint32_t animId_;
	// アニメーションスタート
	bool isStart_;
};