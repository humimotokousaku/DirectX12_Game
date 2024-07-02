#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"

struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};

// 積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
// 単位Quaternion
Quaternion IdentityQuaternion();
// 共役Quaternionを返す
Quaternion Conjugate(const Quaternion& quaternion);
// Quaternionのnormを返す
float Norm(const Quaternion& quaternion);
// 正規化したQuaternionを返す
Quaternion Normalize(const Quaternion& quaternion);
// 逆Quaternionを返す
Quaternion Inverse(const Quaternion& quaternion);
// 内積
float Dot(Quaternion q0, Quaternion q1);
// オイラー角からクォータニオンに変換
Quaternion EulerToQuaternion(Vector3 rotate);
// 任意軸回転を表すQuaternionの生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& vector, Quaternion quaternion);
// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);
// 回転角がクォータニオンでAffine行列を作成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);

// 球面線形補間
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);