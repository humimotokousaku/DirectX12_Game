#pragma once
#include <numbers> 

struct Vector3 final {
	float x;
	float y;
	float z;

	// 算術演算子のオーバーロード
	// 加算
	Vector3 operator+(const Vector3& obj)const {
		return  { x + obj.x, y + obj.y, z + obj.z };
	}
	Vector3 operator+=(const Vector3& obj) {
		return *this = *this + obj;
	}
	Vector3 operator+(float obj)const {
		return  { x + obj, y + obj, z + obj };
	}
	Vector3 operator+=(float obj) {
		return *this = *this + obj;
	}

	// 減算
	Vector3 operator-(const Vector3& obj)const {
		return  { x - obj.x, y - obj.y, z - obj.z };
	}
	Vector3 operator-=(const Vector3& obj) {
		return *this = *this - obj;
	}
	Vector3 operator-(float obj)const {
		return  { x - obj, y - obj, z - obj };
	}
	Vector3 operator-=(float obj) {
		return *this = *this - obj;
	}

	// 掛け算
	Vector3 operator*(const Vector3& obj)const {
		return  { x * obj.x, y * obj.y, z * obj.z };
	}
	Vector3 operator*=(const Vector3& obj) {
		return *this = *this * obj;
	}
	Vector3 operator*(float scalar)const {
		return  { x * scalar, y * scalar, z * scalar };
	}
	Vector3 operator*=(float scalar) {
		return *this = *this * scalar;
	}

	// 割り算
	Vector3 operator/(const Vector3& obj)const {
		return  { x / obj.x, y / obj.y, z / obj.z };
	}
	Vector3 operator/=(const Vector3& obj) {
		return *this = *this / obj;;
	}
	Vector3 operator/(float obj)const {
		return  { x / obj, y / obj, z / obj };
	}
	Vector3 operator/=(float obj) {
		return *this = *this / obj;;
	}
};

// 加算
Vector3 Add(const Vector3& pos, const Vector3& vector);
// 減算
Vector3 Subtract(const Vector3& pos1, const Vector3& pos2);
// 積
Vector3 Multiply(const Vector3& v1, const Vector3& v2);
// スカラー倍
Vector3 Multiply(const float& scalar, const Vector3& v);

// 内積
float Dot(const Vector3& v1, const Vector3& v2);
// 長さ(ノルム)
float Length(const Vector3& v);

// 正規化
Vector3 Normalize(const Vector3& v);

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);

// 度数から弧度に変換
float Degree2Radian(float degree);

// 弧度から度数に変換
float Radian2Degree(float radian);