#include "Vector4.h"

Vector4::Vector4(double x, double y, double z, double w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector4::Vector4() {
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

Vector4::~Vector4() {

}

Vector4 Vector4::operator+ (const Vector4& v2) {
	Vector4 retVector;
	retVector.x = x + v2.x;
	retVector.y = y + v2.y;
	retVector.z = z + v2.z;
	retVector.w = w + v2.w;
	return retVector;
}

Vector4 Vector4::operator- (const Vector4& v2) {
	Vector4 retVector;
	retVector.x = x - v2.x;
	retVector.y = y - v2.y;
	retVector.z = z - v2.z;
	retVector.w = w - v2.w;
	return retVector;
}

void Vector4::print(string comment) {
	cout << comment << ":" << "(x,y,z,w) = (" << x << "," << y << "," << z << "," << w << ")" << endl;
}

void Vector4::dehomogenize() {
	if (w != 0) {
		x = x / w;
		y = y / w;
		z = z / w;
		w = 1;
	}
}

// If the indi is out of bounds, then it just returns the x value.
double Vector4::get(int indi) const {
	switch (indi) {
	case 0: return x;
	case 1: return y;
	case 2: return z;
	case 3: return w;
	default: cerr << "invalid get(Vector4) index" << endl; exit(0);
	}
}

void Vector4::set(int indi, double val) {
	switch (indi) {
	case 0: x = val; break;
	case 1: y = val; break;
	case 2: z = val; break;
	case 3: w = val; break;
	default: cerr << "invalid set(Vector4) index" << endl; exit(0);
	}
}
