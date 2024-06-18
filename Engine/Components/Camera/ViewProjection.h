#pragma once
#include "DirectXCommon.h"
#include "MathStructs.h"
#include "ModelStructs.h"
#include <d3d12.h>
#include <wrl.h>

// 定数バッファ用データ構造体
struct ConstBufferDataViewProjection {
	Matrix4x4 view;       // ワールド → ビュー変換行列
	Matrix4x4 projection; // ビュー → プロジェクション変換行列
	Vector3 cameraPos;    // カメラ座標（ワールド座標）
};

struct ViewProjection{
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// マッピング済みアドレス
	ConstBufferDataViewProjection* constMap = nullptr;

#pragma region ビュー行列の設定
	// トランスフォーム
	/*Transform transform = {
		{1,1,1},
		{0,0,0},
		{0,0,-10}
	};*/
	Vector3 translate = { 0,0,10 };
	Vector3 rotate;
	Vector3 scale = { 1,1,1 };
#pragma endregion

#pragma region 射影行列の設定
	// 垂直方向視野角
	float fovAngleY = 45.0f * 3.141592654f / 180.0f;
	// ビューポートのアスペクト比
	float aspectRatio = (float)16 / 9;
	// 深度限界（手前側）
	float nearZ = 0.1f;
	// 深度限界（奥側）
	float farZ = 1000.0f;
#pragma endregion

	// ビュー行列
	Matrix4x4 matView;
	// 射影行列
	Matrix4x4 matProjection;
};