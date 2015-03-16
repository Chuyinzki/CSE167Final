#ifndef _MATRIX4_H_
#define _MATRIX4_H_

#include <string>
#include "Vector3.h"
#include "Vector4.h"
using namespace std;

#define M_PI 3.14159265358979323846

class Matrix4 {
  public:
    double m[4][4];   // matrix elements; first index is for rows, second for columns (row-major)
    
  public:
	  Matrix4();
	  Matrix4(double, double, double, double, double, double, double, double,
		  double, double, double, double, double, double, double, double);
    Matrix4& operator=(const Matrix4& m2);
	Matrix4 operator*(const Matrix4& m2);
	Vector4 operator*(const Vector4& v);
	void identity();
	void transpose();

	void makeRotateX(double angle);
	void makeRotateY(double angle);
	void makeRotateZ(double angle);
	void makeRotate(double angle, Vector3& axis);
	void makeScale(double sx, double sy, double sz);
	void makeTranslate(double tx, double ty, double tz);
	void print(string comment);
    double* getPointer();
	void printPosition();
	Vector4 getColumn(int i);
};

#endif