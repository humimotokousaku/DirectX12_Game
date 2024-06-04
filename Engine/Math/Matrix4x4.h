#pragma once
#include "Vector3.h"

struct Matrix4x4 {
	float m[4][4];
};

// 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
// 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);
// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

#pragma region SRT

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radius);
// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radius);
// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radius);
// 回転行列を作成
Matrix4x4 MakeRotateMatrix(const Vector3& radian);

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

#pragma endregion

// 単位行列の生成
Matrix4x4 MakeIdentity4x4();

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m);

// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// ベクトルに変換
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

#pragma region カメラ計算

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

#pragma endregion