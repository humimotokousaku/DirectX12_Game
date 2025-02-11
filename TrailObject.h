#pragma once
#include "MathStructs.h"
#include <vector>

// トレイルの情報
struct TrailData {
	Vector3 head;
	Vector3 tail;
	bool isUsed;
};

// トレイルのバッファ
struct TrailBuffer {
	Vector3 pos;
	Vector2 uv;
};

/// <summary>
/// トレイルを表示する
/// </summary>
class TrailObject {
public:
	TrailObject(int bufferSize);
	~TrailObject() = default;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 現在のフレームのトレイル情報を保存
	/// </summary>
	/// <param name="head"></param>
	/// <param name="tail"></param>
	void SetPos(const Vector3& head, const Vector3& tail);

	/// <summary>
	/// 現在のトレイル情報
	/// </summary>
	/// <returns></returns>
	std::vector<TrailData> GetUsedPosArray();

private:// プライベートなメンバ変数
	// トレイルの情報
	std::vector<TrailData> posArray_;
	// 頂点バッファ
	std::vector<TrailBuffer> vertex_;
	// 現在のフレームでのトレイル情報
	TrailData currentPos_;

	// 頂点情報
	//std::vector<TrailBuffer> vertex_;
};