#pragma once
#pragma comment (lib, "d3d9.lib")

#include "Imgui/imgui.h"
#include <emmintrin.h>
#include <immintrin.h>
#include <d3d9.h>
#include <corecrt_math.h>
#include <cstdint>
#include "driver.h"

int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f)
	{

	}

	Vector2(double _x, double _y) : x(_x), y(_y)
	{

	}
	~Vector2()
	{

	}

	double x;
	double y;

	inline bool IsInScreen()
	{
		if (((x <= 0 || x > ScreenWidth) && (y <= 0 || y > ScreenHeight)) || ((x <= 0 || x > ScreenWidth) || (y <= 0 || y > ScreenHeight))) {
			return false;
		}
		else {
			return true;
		}
	}
};
class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	double x;
	double y;
	double z;

	inline double Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline bool isValid() const {
		return isfinite(x) && isfinite(y) && isfinite(z);
	}

	inline double Distance(Vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	inline double Length() {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3 Cross(const Vector3& other) const {
		return Vector3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	Vector3 Normalize() const {
		float length = sqrt(x * x + y * y + z * z);
		if (length > 0.0f) {
			return Vector3(x / length, y / length, z / length);
		}
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(const Vector3& v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	static Vector3 CalcAngle(const Vector3& src, const Vector3& dst) {
		Vector3 delta = dst - src;
		float hypotenuse = delta.Length();

		double pi = 3.14159265359;
		float yaw = std::atan2(delta.y, delta.x) * (180.0f / pi);
		float pitch = std::asin(delta.z / hypotenuse) * (180.0f / pi);
		return { pitch, yaw, 0.0f };
	}

	ImVec2 AsImVec2() const {
		return ImVec2(x, y);
	}

	Vector3 operator*(double flNum) { return Vector3(x * flNum, y * flNum, z * flNum); }
};

class Vector4
{
public:
	Vector4() : x(0.0), y(0.0), z(0.0), w(0.0)
	{

	}

	Vector4(double _x, double _y, double _z, double _w) : x(_x), y(_y), z(_z), w(_w)
	{

	}

	~Vector4()
	{

	}

	double x;
	double y;
	double z;
	double w;

	inline double Dot(Vector4 v)
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	inline bool isValid() const
	{
		return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w);
	}

	inline double Distance(Vector4 v)
	{
		return std::sqrt(std::pow(v.x - x, 2.0) + std::pow(v.y - y, 2.0) + std::pow(v.z - z, 2.0) + std::pow(v.w - w, 2.0));
	}

	inline double Length() const
	{
		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	Vector4 Normalize() const
	{
		double length = Length();
		if (length == 0) {
			return Vector4(0.0, 0.0, 0.0, 0.0); 
		}
		return Vector4(x / length, y / length, z / length, w / length);
	}

	Vector4 operator+(Vector4 v)
	{
		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	Vector4 operator-(Vector4 v)
	{
		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	Vector4 operator*(double flNum)
	{
		return Vector4(x * flNum, y * flNum, z * flNum, w * flNum);
	}
};

double ToRad(double degree)
{
	double pi = 3.14159265359;
	return (degree * (pi / 180));
}
Vector4 ToQuat(Vector3 Euler) {

	double heading = ToRad(Euler.x);
	double attitude = ToRad(Euler.y);
	double bank = ToRad(Euler.z);

	double c1 = cos(heading / 2);
	double s1 = sin(heading / 2);
	double c2 = cos(attitude / 2);
	double s2 = sin(attitude / 2);
	double c3 = cos(bank / 2);
	double s3 = sin(bank / 2);
	double c1c2 = c1 * c2;
	double s1s2 = s1 * s2;
	Vector4 Quat;
	Quat.w = c1c2 * c3 - s1s2 * s3;
	Quat.x = c1c2 * s3 + s1s2 * c3;
	Quat.y = s1 * c2 * c3 + c1 * s2 * s3;
	Quat.z = c1 * s2 * c3 - s1 * c2 * s3;
	Vector4 Quat2;
	Quat2 = { Quat.y, Quat.z, (Quat.x * -1), Quat.w };
	return Quat2;
}

struct FRotator
{
	FRotator() : Pitch(), Yaw(), Roll() { }
	FRotator(double Pitch, double Yaw, double Roll) : Pitch(Pitch), Yaw(Yaw), Roll(Roll) { }

	FRotator operator + (const FRotator& other) const { return { this->Pitch + other.Pitch, this->Yaw + other.Yaw, this->Roll + other.Roll }; }
	FRotator operator - (const FRotator& other) const { return { this->Pitch - other.Pitch, this->Yaw - other.Yaw, this->Roll - other.Roll }; }
	FRotator operator * (double offset) const { return { this->Pitch * offset, this->Yaw * offset, this->Roll * offset }; }
	FRotator operator / (double offset) const { return { this->Pitch / offset, this->Yaw / offset, this->Roll / offset }; }

	FRotator& operator *= (const double other) { this->Pitch *= other; this->Yaw *= other; this->Roll *= other; return *this; }
	FRotator& operator /= (const double other) { this->Pitch /= other; this->Yaw /= other; this->Roll /= other; return *this; }

	FRotator& operator = (const FRotator& other) { this->Pitch = other.Pitch; this->Yaw = other.Yaw; this->Roll = other.Roll; return *this; }
	FRotator& operator += (const FRotator& other) { this->Pitch += other.Pitch; this->Yaw += other.Yaw; this->Roll += other.Roll; return *this; }
	FRotator& operator -= (const FRotator& other) { this->Pitch -= other.Pitch; this->Yaw -= other.Yaw; this->Roll -= other.Roll; return *this; }
	FRotator& operator *= (const FRotator& other) { this->Pitch *= other.Pitch; this->Yaw *= other.Yaw; this->Roll *= other.Roll; return *this; }
	FRotator& operator /= (const FRotator& other) { this->Pitch /= other.Pitch; this->Yaw /= other.Yaw; this->Roll /= other.Roll; return *this; }

	operator bool() { return bool(this->Pitch || this->Yaw || this->Roll); }
	friend bool operator == (const FRotator& a, const FRotator& b) { return a.Pitch == b.Pitch && a.Yaw == b.Yaw && a.Roll == b.Roll; }
	friend bool operator != (const FRotator& a, const FRotator& b) { return !(a == b); }

	FRotator operator/(double Scalar)
	{
		return FRotator(this->Pitch / Scalar, this->Yaw / Scalar, this->Roll / Scalar);
	}

	Vector3 Euler() const
	{
		return Vector3(Pitch, Yaw, Roll);
	}

	FRotator Normalize()
	{
		while (this->Yaw > 180.0)
			this->Yaw -= 360.0;
		while (this->Yaw < -180.0)
			this->Yaw += 360.0;

		while (this->Pitch > 180.0)
			this->Pitch -= 360.0;
		while (this->Pitch < -180.0)
			this->Pitch += 360.0;

		this->Roll = 0.0;
		return *this;
	}

	double Pitch, Yaw, Roll;
};

#define M_PI 3.14159265358979323846f

FRotator VectorToFRotator(const Vector3& vec) {
	FRotator rotator(0.0f, 0.0f, 0.0f);

	rotator.Yaw = atan2(vec.y, vec.x) * (180.0f / M_PI);
	rotator.Pitch = atan2(vec.z, sqrt(vec.x * vec.x + vec.y * vec.y)) * (180.0f / M_PI);
	rotator.Roll = 0.0f; 

	return rotator;
}

class FBoxSphereBounds
{
public:
	Vector3 Origin;
	Vector3 BoxExtent;
	double SphereRadius;
};

struct FQuat
{
	double x;
	double y;
	double z;
	double w;
};

struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}