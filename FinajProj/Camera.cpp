#include "Camera.h"


Camera::Camera()
{
	reset();
}


Camera::~Camera()
{

}

//row order
void Camera::setWorld2Camera() //set the camera matrix
{
	//update C and return it.
	int height = 4;
	world2camera.identity();
	for (int i = 0; i < height - 1; i++) //replace top 3 rows of camera matrix (lecture slide 25:projection)
	{
		world2camera.m[i][0] = Xc.getIndex(i);
		world2camera.m[i][1] = Yc.getIndex(i);
		world2camera.m[i][2] = Zc.getIndex(i);
		world2camera.m[i][3] = e.getIndex(i);
	}
}

//row order
void Camera::setCamera2World() //set the inverse camera matrix
{
	setWorld2Camera();

	Matrix4 rotationInverse = world2camera;
	rotationInverse.m[0][3] = 0;
	rotationInverse.m[1][3] = 0;
	rotationInverse.m[2][3] = 0;
	rotationInverse.transpose(); //finished

	Matrix4 translationInverse;
	translationInverse.identity();
	double sx = world2camera.m[0][3];
	double sy = world2camera.m[1][3];
	double sz = world2camera.m[2][3];
	translationInverse.makeTranslate(-sx, -sy, -sz); //finished

	camera2world = rotationInverse*translationInverse; //C^(-1) = R^(-1) * T^(-1)
}

//row order
Matrix4 Camera::getWorld2Camera() //return the camera matrix
{
	return world2camera;
}

//row order
Matrix4 Camera::getCamera2World() //return the inverse camera matrix
{
	return camera2world;
}

//column order.
Matrix4 Camera::getGLMatrix()
{
	Matrix4 glMatrix = camera2world;
	glMatrix.transpose(); //turn it into column order.
	return glMatrix;
}

void Camera::setE(double x, double y, double z)
{
	e = Vector3(x, y, z);
	setCameraAxis();
	setCamera2World();
}

void Camera::setD(double x, double y, double z)
{
	d = Vector3(x, y, z);
	setCameraAxis();
	setCamera2World();
}

void Camera::setUp(double x, double y, double z)
{
	up = Vector3(x, y, z);
	setCameraAxis();
	setCamera2World();
}
Vector3 Camera::calculateUpVector() //assuming x axis is (1,0,0)
{
	Vector3 result;
	Vector3 xAxis(1, 0, 0);
	result = d - e;
	result.normalize();
	result.cross(xAxis, result);
	result.normalize();
	return result;
}

//http://ivl.calit2.net/wiki/images/9/92/03_ProjectionF14.pdf
void Camera::setCameraAxis()
{
	Zc = e - d; Zc.normalize();
	Xc.cross(up, Zc); Xc.normalize();
	Yc.cross(Zc, Xc);
}

Vector3 Camera::getXAxis(){return Xc;}
Vector3 Camera::getYAxis(){return Yc;}
Vector3 Camera::getZAxis(){return Zc;}

Vector3 Camera::getE() { return e; }
Vector3 Camera::getD(){ return d; }
Vector3 Camera::getUp(){ return up; }

void Camera::reset()
{
	//identity camera matrix.
	setE(0, 0, 0);
	setD(0, 0, -1);
	setUp(0, 1, 0);
}