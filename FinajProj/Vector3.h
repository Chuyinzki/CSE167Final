#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include <string>
#include <iostream>
 using namespace std;

class Vector3 {
public:
	double x, y, z;
public:
	Vector3(double x, double y, double z);
	Vector3();
	Vector3 operator+(const Vector3& v2);
	Vector3 operator-(const Vector3& v2);
	Vector3 operator=(const Vector3& v2);
	Vector3 operator/(const double divisor);
	Vector3 Vector3::operator*(const double multiplier);
	void set(double x, double y, double z);
	void negate();
	void scale(double s);
	double dot(const Vector3& v1, const Vector3& v2);
	void cross(const Vector3& v1, const Vector3& v2);
	Vector3 crossj(const Vector3& v1, const Vector3& v2);
	double length();
	void normalize();
	void print(string comment);
	~Vector3();

	//setters and getters
	double getX(){ return x; }
	double getY(){ return y; }
	double getZ(){ return z; }
	double getIndex(int index);
	double setX(double x){ this->x = x; }
	double setY(double y){ this->y = y; }
	double setZ(double z){ this->z = z; }
};

#endif

