#pragma once
#include <math.h>

struct Vector2 {
	float x;
	float y;

	// 算術演算子のオーバーロード
	// 加算
	Vector2 operator+(const Vector2& obj)const {
		return  { x + obj.x, y + obj.y };
	}
	Vector2 operator+=(const Vector2& obj) {
		return *this = *this + obj;
	}
	Vector2 operator+(float obj)const {
		return  { x + obj, y + obj };
	}
	Vector2 operator+=(float obj) {
		return *this = *this + obj;
	}

	// 減算
	Vector2 operator-(const Vector2& obj)const {
		return  { x - obj.x, y - obj.y };
	}
	Vector2 operator-=(const Vector2& obj) {
		return *this = *this - obj;
	}
	Vector2 operator-(float obj)const {
		return  { x - obj, y - obj };
	}
	Vector2 operator-=(float obj) {
		return *this = *this - obj;
	}

	// 掛け算
	Vector2 operator*(const Vector2& obj)const {
		return  { x * obj.x, y * obj.y };
	}
	Vector2 operator*=(const Vector2& obj) {
		return *this = *this * obj;
	}
	Vector2 operator*(float scalar)const {
		return  { x * scalar, y * scalar };
	}
	Vector2 operator*=(float scalar) {
		return *this = *this * scalar;
	}

	// 割り算
	Vector2 operator/(const Vector2& obj)const {
		return  { x / obj.x, y / obj.y };
	}
	Vector2 operator/=(const Vector2& obj) {
		return *this = *this / obj;;
	}
	Vector2 operator/(float obj)const {
		return  { x / obj, y / obj };
	}
	Vector2 operator/=(float obj) {
		return *this = *this / obj;;
	}
};

// 内積
float Dot(Vector2 v1, Vector2 v2);

// 長さ
float Length(Vector2 v);

// 正規化
Vector2 Normalize(const Vector2& v);