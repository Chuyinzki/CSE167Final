#include <math.h>
#include "Matrix4.h"
#include <assert.h>
Matrix4::Matrix4() {
  for (int i=0; i<4; ++i) {
    for (int j=0; j<4; ++j) {
		if (i == j)
			m[i][j] = 1;
		else
			m[i][j] = 0;
    }
  }
}

Matrix4::Matrix4(double p0x, double p1x, double p2x, double p3x,
	double p0y, double p1y, double p2y, double p3y,
	double p0z, double p1z, double p2z, double p3z,
	double p0w, double p1w, double p2w, double p3w)
{
	m[0][0] = p0x;
	m[0][1] = p1x;
	m[0][2] = p2x;
	m[0][3] = p3x;

	m[1][0] = p0y;
	m[1][1] = p1y;
	m[1][2] = p2y;
	m[1][3] = p3y;

	m[2][0] = p0z;
	m[2][1] = p1z;
	m[2][2] = p2z;
	m[2][3] = p3z;

	m[3][0] = p0w;
	m[3][1] = p1w;
	m[3][2] = p2w;
	m[3][3] = p3w;
}

Vector4 Matrix4::getColumn(int i)
{
	assert(i >= 0 && i < 4);
	return Vector4(m[i][0], m[i][1], m[i][2], m[i][3]);
}
Matrix4& Matrix4::operator=(const Matrix4& m2) {
  if (this != &m2) {
    for (int i=0; i<4; ++i) {
      for (int j=0; j<4; ++j) {
        m[i][j] = m2.m[i][j];
      }
    }
  }
  return *this;
}

// return pointer to matrix elements
double* Matrix4::getPointer() {
  return &m[0][0];
}

// set matrix to identity matrix
void Matrix4::identity() {
  for (int i=0; i<4; ++i) {
    for (int j=0; j<4; ++j) {
      if (i==j) 
        m[i][j] = 1; 
      else 
        m[i][j] = 0;
    }
  }
}

// transpose the matrix (mirror at diagonal)
void Matrix4::transpose() {
  Matrix4 temp;
  //temp.identity();
  for (int i=0; i<4; ++i) {
    for (int j=0; j<4; ++j) {
			temp.m[j][i] = m[i][j];
    }
  }
  *this = temp;  // copy temporary values to this matrix
}

// Creates a rotation matrix which rotates about the X axis.
// angle is expected in degrees
void Matrix4::makeRotateX(double angle) {
	angle = angle / 180.0 * M_PI;  // convert from degrees to radians
	identity();
	m[1][1] = cos(angle);
	m[2][1] = sin(angle);
	m[1][2] = -sin(angle);
	m[2][2] = cos(angle);
}

// Creates a rotation matrix which rotates about the y axis.
// angle is expected in degrees
void Matrix4::makeRotateY(double angle) {
	angle = angle / 180.0 * M_PI;  // convert from degrees to radians
	identity();
	m[0][0] = cos(angle);
	m[0][2] = sin(angle);
	m[2][0] = -sin(angle);
	m[2][2] = cos(angle);
}

// Creates a rotation matrix which rotates about the z axis.
// angle is expected in degrees
void Matrix4::makeRotateZ(double angle) {
	angle = angle / 180.0 * M_PI;  // convert from degrees to radians
	identity();
	m[0][0] = cos(angle);
	m[1][0] = sin(angle);
	m[0][1] = -sin(angle);
	m[1][1] = cos(angle);
}

void Matrix4::makeRotate(double angle, Vector3& axis){
	identity();
	axis.normalize();
	angle = angle / 180.0 * M_PI;  // convert from degrees to radians
	m[0][0] = cos(angle) + axis.x * axis.x * (1 - cos(angle));
	m[0][1] = axis.x * axis.y * (1 - cos(angle)) - axis.z * sin(angle);
	m[0][2] = axis.x * axis.z * (1 - cos(angle)) + axis.y* sin(angle);

	m[1][0] = axis.x * axis.y * (1 - cos(angle)) + axis.z * sin(angle);
	m[1][1] = cos(angle) + axis.y * axis.y * (1 - cos(angle));
	m[1][2] = axis.y * axis.z * (1 - cos(angle)) - axis.x * sin(angle);

	m[2][0] = axis.x * axis.z * (1 - cos(angle)) - axis.y* sin(angle);
	m[2][1] = axis.y * axis.z * (1 - cos(angle)) + axis.x * sin(angle);
	m[2][2] = cos(angle) + axis.y * axis.y * (1 - cos(angle));
	
}

// CURR_STATE: ROW_MAJOR
Matrix4 Matrix4::operator*(const Matrix4& m2){
	Matrix4 retMatrix;
	for (int i = 0; i < 4; i++) { // i is row
		for (int j = 0; j < 4; j++) { // j is col
			double sum = 0;
			for (int k = 0; k < 4; k++) {
				sum += m[i][k] * m2.m[k][j];
			}
			retMatrix.m[i][j] = sum;
		}
	}
	return retMatrix;
}

// CURR_STATE: ROW_MAJOR
Vector4 Matrix4::operator*(const Vector4& v){
	Vector4 retVector;
	for (int i = 0; i < 4; i++) {
		double sum = 0;
		for (int j = 0; j < 4; j++) {
			sum += m[i][j] * v.get(j);
		}
		retVector.set(i, sum);
	}
	return retVector;
}

void Matrix4::makeScale(double sx, double sy, double sz){
	identity();
	m[0][0] = sx;
	m[1][1] = sy;
	m[2][2] = sz;
}

//TODO: figure out if this matrix should be column order or row order. Swap matrix parameters if so.
//assuming w = 1. Might have to scale the vector so that it equals 1 before running this function.
// CURR_STATE: row-major
void Matrix4::makeTranslate(double tx, double ty, double tz){
	identity();
	m[0][3] = tx;
	m[1][3] = ty;
	m[2][3] = tz;
}

//TODO: fix this if he wants us to print column order.
// CURR_STATE: row-rajor
void Matrix4::print(string comment){
	cout << comment << ":" << endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cout << m[i][j] << " ";
			if (j == 3 && i != 3) { cout << endl; } //end the row
		}
		if (i == 3) {cout << ";" << endl; } //end the row
	}
}

void Matrix4::printPosition() {
	Vector3 v(m[0][3], m[1][3], m[2][3]);
	v.print("Current Position: ");
}