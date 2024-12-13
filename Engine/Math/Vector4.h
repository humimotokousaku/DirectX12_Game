#pragma once
struct Vector4 final {
	float x;
	float y;
	float z;
	float w;

	// 算術演算子のオーバーロード
	// 加算
	Vector4 operator+(const Vector4& obj)const {
		return  { x + obj.x, y + obj.y, z + obj.z ,w + obj.w };
	}
	Vector4 operator+=(const Vector4& obj) {
		return *this = *this + obj;
	}
	Vector4 operator+(float obj)const {
		return  { x + obj, y + obj, z + obj, w + obj };
	}
	Vector4 operator+=(float obj) {
		return *this = *this + obj;
	}

	// 減算
	Vector4 operator-(const Vector4& obj)const {
		return  { x - obj.x, y - obj.y, z - obj.z, w - obj.w };
	}
	Vector4 operator-=(const Vector4& obj) {
		return *this = *this - obj;
	}
	Vector4 operator-(float obj)const {
		return  { x - obj, y - obj, z - obj, w - obj };
	}
	Vector4 operator-=(float obj) {
		return *this = *this - obj;
	}
};