#pragma once
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