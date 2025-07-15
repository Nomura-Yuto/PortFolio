//===========================================================================
// vector.h					自作Vectorクラス
// 製作者：野村 悠人
//===========================================================================
#pragma once
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <math.h>
#include "main.h"

// 二次元ベクトルクラス
class Vector2
{
public:
	float x;
	float y;

	//---- 関数 -----
	Vector2() : x(0.0f), y(0.0f) {}
	Vector2(float setX, float setY) : x(setX), y(setY) {}

	// 内積
	float Dot(const Vector2& v) const {
		return (x * v.x + y * v.y);
	}
	static float Dot(const Vector2& a, const Vector2& b) {
		return (a.x * b.x + a.y * b.y);
	}

	// 外積
	float Cross(const Vector2& v) const {
		return (x * v.y - y * v.x);
	}
	static float Cross(const Vector2& a, const Vector2& b) {
		return (a.x * b.y - a.y * b.x);
	}

	// 正規化
	Vector2 Normalize(void) const {
		float len = Length();
		if (len == 0.0f) return Vector2(0.0f, 0.0f);
		return Vector2(x, y) * len;
	}
	static Vector2 Normalize(const Vector2& vec) {
		float len = Length(vec);
		if (len == 0.0f) return Vector2(0.0f, 0.0f);
		return Vector2(vec.x, vec.y) / len;
	}

	// ベクトル長
	float Length(void) const {
		return sqrtf(x * x + y * y);
	}
	static float Length(const Vector2& vec) {
		return sqrtf(vec.x * vec.x + vec.y * vec.y);
	}
	// ベクトル長の2乗
	float LengthSqr(void) const {
		return (x * x + y * y);
	}

	// 反射ベクトル計算
	Vector2 Reflect(const Vector2& normal) const;
	// 屈折ベクトル計算
	Vector2 Refract(const Vector2& normal) const;

	//---- operator -----
	Vector2 operator=(const Vector2& v) {
		return Vector2(x = v.x, y = v.y);
	}
	bool operator==(const Vector2& v) const {
		return(x == v.x) && (y == v.y);
	}
	bool operator!=(const Vector2& v) const {
		return(x != v.x) || (y != v.y);
	}

	Vector2 operator+(const Vector2& v) const {
		return Vector2(x + v.x, y + v.y);
	}
	Vector2 operator-() const {
		return Vector2(-x, -y);
	}
	Vector2 operator-(const Vector2& v) const {
		return Vector2(x - v.x, y - v.y);
	}
	Vector2 operator*(float f) const {
		return Vector2(x * f, y * f);
	}
	Vector2 operator*(const Vector2& v) const {
		return Vector2(x * v.x, y * v.y);
	}
	friend Vector2 operator*(float f, const Vector2& v) {
		return Vector2(f * v.x, f * v.y);
	}
	Vector2 operator/(float f) const {
		return Vector2(x / f, y / f);
	}
	Vector2 operator/(const Vector2& v) const {
		return Vector2(x / v.x, y / v.y);
	}

	Vector2 operator+=(const Vector2& v) {
		return Vector2(x += v.x, y += v.y);
	}
	Vector2 operator-=(const Vector2& v) {
		return Vector2(x -= v.x, y -= v.y);
	}
	Vector2 operator*=(const Vector2& v) {
		return Vector2(x *= v.x, y *= v.y);
	}
	Vector2 operator*=(float f) {
		return Vector2(x *= f, y *= f);
	}
	Vector2 operator/=(const Vector2& v) {
		return Vector2(x /= v.x, y /= v.y);
	}
	Vector2 operator/=(float f) {
		return Vector2(x /= f, y /= f);
	}

	Vector2(DirectX::XMFLOAT2 v) : x(v.x), y(v.y) {}
	operator DirectX::XMFLOAT2() const { return { x, y }; }
};

// 三次元ベクトルクラス
class Vector3
{
public:
	float x;
	float y;
	float z;

	//---- 関数 -----
	Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3(float setX, float setY, float setZ) : x(setX), y(setY), z(setZ) {}

	// 内積
	float Dot(const Vector3& v) const {
		return (x * v.x + y * v.y + z * v.z);
	}
	static float Dot(const Vector3& a, const Vector3& b) {
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}

	// 外積
	Vector3 Cross(const Vector3& v) const {
		return Vector3(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		);
	}
	static Vector3 Cross(const Vector3& a, const Vector3& b) {
		return Vector3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}

	// 正規化
	Vector3 Normalize(void) const {
		float len = Length();
		if (len == 0.0f) return Vector3(0.0f, 0.0f, 0.0f);
		return Vector3(x, y, z) / len;
	}
	static Vector3 Normalize(const Vector3& vec) {
		float len = Length(vec);
		if (len == 0.0f) return Vector3(0.0f, 0.0f, 0.0f);
		return Vector3(vec.x, vec.y, vec.z) / len;
	}

	// ベクトル長
	float Length(void) const {
		return sqrtf(x * x + y * y + z * z);
	}
	static float Length(const Vector3& vec) {
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}
	// ベクトル長の2乗
	float LengthSqr(void) const {
		return (x * x + y * y + z * z);
	}

	// 反射ベクトル計算
	Vector3 Reflect(const Vector3& normal) const;
	// 屈折ベクトル計算
	Vector3 Refract(const Vector3& normal) const;

	//---- operator -----
	Vector3 operator=(const Vector3& v) {
		return Vector3(x = v.x, y = v.y, z = v.z);
	}
	bool operator==(const Vector3& v)const {
		return(x == v.x) && (y == v.y) && (z == v.z);
	}
	bool operator!=(const Vector3& v)const {
		return(x != v.x) || (y != v.y) || (z != v.z);
	}

	Vector3 operator+(const Vector3& v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	Vector3 operator-() const {
		return Vector3(-x, -y, -z);
	}
	Vector3 operator-(const Vector3& v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	Vector3 operator*(float f) const {
		return Vector3(x * f, y * f, z * f);
	}
	friend Vector3 operator*(float f, const Vector3& v) {
		return Vector3(f * v.x, f * v.y, f * v.z);
	}
	Vector3 operator/(float f) const {
		return Vector3(x / f, y / f, z / f);
	}

	Vector3 operator+=(const Vector3& v) {
		return Vector3(x += v.x, y += v.y, z += v.z);
	}
	Vector3 operator-=(const Vector3& v) {
		return Vector3(x -= v.x, y -= v.y, z -= v.z);
	}
	Vector3 operator*=(float f) {
		return Vector3(x *= f, y *= f, z *= f);
	}
	Vector3 operator/=(float f) {
		return Vector3(x /= f, y /= f, z /= f);
	}

	Vector3(DirectX::XMFLOAT3 v) : x(v.x), y(v.y), z(v.z) {}
	operator DirectX::XMFLOAT3() const { return { x, y, z }; }
};


// 四次元ベクトルクラス
class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	//---- 関数 -----
	Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	Vector4(float setX, float setY, float setZ, float setW) : x(setX), y(setY), z(setZ), w(setW) {}

	// 内積
	float Dot(const Vector4& v) const {
		return (x * v.x + y * v.y + z * v.z);
	}
	static float Dot(const Vector4& a, const Vector4& b) {
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}

	// 正規化
	Vector4 Normalize(void) const {
		return (Vector4(x, y, z, w) / Length());
	}

	// ベクトル長
	float Length(void) const {
		return sqrtf(x * x + y * y + z * z + w * w);
	}
	// ベクトル長の2乗
	float LengthSqr(void) const {
		return (x * x + y * y + z * z + w * w);
	}

	//---- operator -----
	Vector4 operator=(const Vector4& v) {
		return Vector4(x = v.x, y = v.y, z = v.z, w = v.w);
	}
	bool operator==(const Vector4& v) const {
		return(x == v.x) && (y == v.y) && (z == v.z) && (w == v.w);
	}
	bool operator!=(const Vector4& v) const {
		return(x != v.x) || (y != v.y) || (z != v.z) || (w != v.w);
	}

	Vector4 operator+(const Vector4& v) const {
		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}
	Vector4 operator-() const {
		return Vector4(-x, -y, -z, -w);
	}
	Vector4 operator-(const Vector4& v) const {
		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}
	Vector4 operator*(float f) const {
		return Vector4(x * f, y * f, z * f, w * f);
	}
	friend Vector4 operator*(float f, const Vector4& v) {
		return Vector4(f * v.x, f * v.y, f * v.z, f * v.w);
	}
	Vector4 operator/(float f) const {
		return Vector4(x / f, y / f, z / f, w / f);
	}

	Vector4 operator+=(const Vector4& v) {
		return Vector4(x += v.x, y += v.y, z += v.z, w += v.w);
	}
	Vector4 operator-=(const Vector4& v) {
		return Vector4(x -= v.x, y -= v.y, z -= v.z, w -= v.w);
	}
	Vector4 operator*=(float f) {
		return Vector4(x *= f, y *= f, z *= f, w *= f);
	}
	Vector4 operator/=(float f) {
		return Vector4(x /= f, y /= f, z /= f, w /= f);
	}


	Vector4(DirectX::XMFLOAT4 v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
	operator DirectX::XMFLOAT4() const { return { x, y, z, w }; }
};


// カラークラス
class Color
{
public:
	float r;
	float g;
	float b;
	float a;

	//---- 関数 -----
	Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
	Color(float setR, float setG, float setB, float setA) : r(setR), g(setG), b(setB), a(setA) {}


	// 線形補完
	Color Lerp(const Color& c, float t) {
		t = ClampValue(t, 0.0f, 1.0f);
		return Color(
			(1 - t) * r + t * c.r,
			(1 - t) * g + t * c.g,
			(1 - t) * b + t * c.b,
			(1 - t) * a + t * c.a
		);
	}

	// 範囲制限
	float ClampValue(float value, float min = 0.0f, float max = 0.0f) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}
	Color Clamp(float min = 0.0f, float max = 1.0f) {
		r = ClampValue(r, min, max);
		g = ClampValue(g, min, max);
		b = ClampValue(b, min, max);
		a = ClampValue(a, min, max);
	}

	// 正規化
	void Normalize() {
		float m = sqrt(r * r + g * g + b * b);
		if (m > 0.0f) {
			r /= m;
			g /= m;
			b /= m;
		}
	}


	//---- operator -----
	Color operator=(const Color& c) {
		return Color(r = c.r, g = c.g, b = c.b, a = c.a);
	}
	bool operator==(const Color& c) const {
		return(r == c.r) && (g == c.g) && (b == c.b) && (a == c.a);
	}
	bool operator!=(const Color& c) const {
		return(r != c.r) || (g != c.g) || (b != c.b) || (a != c.a);
	}

	Color operator+(const Color& c) const {
		return Color(r + c.r, g + c.g, b + c.b, a + c.a);
	}
	Color operator-() const {
		return Color(-r, -g, -b, -a);
	}
	Color operator-(const Color& c) const {
		return Color(r - c.r, g - c.g, b - c.b, a - c.a);
	}
	Color operator*(const Color& c) const {
		return Color(r * c.r, g * c.g, b * c.b, a * c.a);
	}
	Color operator*(float f) const {
		return Color(r * f, g * f, b * f, a * f);
	}
	friend Color operator*(float f, const Color& c) {
		return Color(f * c.r, f * c.g, f * c.b, f * c.a);
	}
	Color operator/(float f) const {
		return Color(r / f, g / f, b / f, a / f);
	}

	Color operator+=(const Color& c) {
		return Color(r += c.r, g += c.g, b += c.b, a += c.a);
	}
	Color operator-=(const Color& c) {
		return Color(r -= c.r, g -= c.g, b -= c.b, a -= c.a);
	}
	Color operator*=(float f) {
		return Color(r *= f, g *= f, b *= f, a *= f);
	}
	Color operator/=(float f) {
		return Color(r /= f, g /= f, b /= f, a /= f);
	}


	Color(DirectX::XMFLOAT4 v) : r(v.x), g(v.y), b(v.z), a(v.w) {}
	operator DirectX::XMFLOAT4() const { return { r, g, b, a }; }
};



void StoreVector3(Vector3* pStrLoc, XMVECTOR& v);
XMVECTOR LoadVector3(Vector3* pSource);


#endif // !_VECTOR_H_

