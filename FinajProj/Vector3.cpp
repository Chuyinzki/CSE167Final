#include "Vector3.h"
#include <assert.h>

Vector3::Vector3(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

//default constructor
Vector3::Vector3() {
	x = 0;
	y = 0;
	z = 0;
}


Vector3 Vector3::operator+(const Vector3& v2) {
	Vector3 retVector;
	retVector.x = x + v2.x;
	retVector.y = y + v2.y;
	retVector.z = z + v2.z;
	return retVector;
}

Vector3 Vector3::operator-(const Vector3& v2) {
	Vector3 retVector = Vector3(0, 0, 0);
	retVector.x = x - v2.x;
	retVector.y = y - v2.y;
	retVector.z = z - v2.z;
	return retVector;
}

Vector3 Vector3::operator=(const Vector3& v2) {
	if (this != &v2) {
		x = v2.x;
		y = v2.y;
		z = v2.z;
	}
	return *this;
}

Vector3 Vector3::operator/(const double divisor)
{
	Vector3 retVector;
	if (divisor != 0)
	{
		retVector.x = x / divisor;
		retVector.y = y / divisor;
		retVector.z = z / divisor;
	}
	return retVector;
	assert(false); //divided by zero.
}

Vector3 Vector3::operator*(const double multiplier)
{
	Vector3 retVector;
	retVector.x = x * multiplier;
	retVector.y = y * multiplier;
	retVector.z = z * multiplier;
	return retVector;
}

void Vector3::negate() {
	x = -1 * x;
	y = -1 * y;
	z = -1 * z;
}

void Vector3::scale(double s) {
	x = s * x;
	y = s * y;
	z = s * z;
}

double Vector3::dot(const Vector3& v1, const Vector3& v2) {
	double xSum = v1.x * v2.x;
	double ySum = v1.y * v2.y;
	double zSum = v1.z * v2.z;
	return xSum + ySum + zSum;
}

void Vector3::cross(const Vector3& v1, const Vector3& v2) {
	x = v1.y * v2.z - v1.z * v2.y;
	y = v1.z * v2.x - v1.x * v2.z;
	z = v1.x * v2.y - v1.y * v2.x;
}

Vector3 Vector3::crossj(const Vector3& a, const Vector3& b) {
	double newX = (a.y*b.z) - (a.z*b.y);
	double newY = (a.z*b.x) - (a.x*b.z);
	double newZ = (a.x*b.y) - (a.y*b.x);
	return Vector3(newX, newY, newZ); //potentially have to change
}

double Vector3::length() {
	return sqrt(x*x + y*y + z*z);
}

void Vector3::normalize() {
	double len = length();
	x = x / len;
	y = y / len;
	z = z / len;
}

void Vector3::set(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}


void Vector3::print(string comment) {
	cout << comment << ":" << "(x,y,z) = (" << x << "," << y << "," << z << ")" << endl;
}

double Vector3::getIndex(int index)
{
	switch (index)
	{
		case 0:
			return x;
			break;
		case 1:
			return y;
			break;
		case 2:
			return z;
			break;
		default:
			assert(false); //requested invalid index.
	}
}
Vector3::~Vector3() {

}