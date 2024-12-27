#pragma once
#include "MathStructs.h"

// ブラーの情報
struct RadialBlurData {
	bool isActive;	 // 使用するか
	float blurWidth; // ブラーの強さ
	Vector2 center;
};

// ヴィネットの情報
struct VignetingData {
	bool isActive;	 // 使用するか
	float scale; // ヴィネットの強さ
};

// ガウスの情報
struct GaussData {
	bool isActive;
	float strength;
};

// dissolveの情報
struct DissolveData {
	bool isActive;// 使用するか
	float maskThreshold;// ぼかすの閾値
	Vector2 edgeRange;
	Vector3 color;
};
struct DissolveDataForGPU {
	bool isActive;// 使用するか
	float maskThreshold;// ぼかすの閾値
};

// ブルームの情報
struct BloomData {
	bool isActive;	// 使用するか
	float strength;	// ぼかしの強さ
	float threshold; // ぼかすの閾値
};