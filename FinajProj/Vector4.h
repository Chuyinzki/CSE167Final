#ifndef _VECTOR4_H_
#define _VECTOR4_H_

#include <string>
#include <iostream>
 using namespace std;

class Vector4 {
private:
	double x, y, z, w;
public:
	Vector4(double x, double y, double z, double w);
	Vector4();
	~Vector4();
	Vector4 operator+(const Vector4& v2);
	Vector4 operator-(const Vector4& v2);
	void print(string comment);
	void dehomogenize();
	void set(int indi, double val);
	double get(int indi) const;
};

#endif
