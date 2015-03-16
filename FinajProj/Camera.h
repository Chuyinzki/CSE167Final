#ifndef _CAMERA_
#define _CAMERA_
#include "Vector3.h"
#include "Matrix4.h"
#include <GL/glut.h>
class Camera
{
private:
	Vector3 e; //center of projection
	Vector3 d; //"look at point"
	Vector3 up; //up vector

	Vector3 Xc; //camera x-axis
	Vector3 Yc; //camera y-axis
	Vector3 Zc; //camera z-axis
	Matrix4 camera2world; //the inverse camera matrix
	Matrix4 world2camera; //the camera matrix ("C")
	void setCameraAxis();
public:
	Camera();
	~Camera();
	Matrix4 getWorld2Camera();
	Matrix4 getCamera2World();
	Matrix4 getGLMatrix();
	void setWorld2Camera();
	void setCamera2World();
	//setters:
	void setE(double x, double y, double z); //center of projection
	void setD(double x, double y, double z); //"look at point"
	void setUp(double x, double y, double z); //up vector
	Vector3 getE();
	Vector3 getD();
	Vector3 getUp();
	void reset(); //reset all variables to default
	Vector3 getXAxis();
	Vector3 getYAxis();
	Vector3 getZAxis();

	Vector3 Camera::calculateUpVector(); //assuming x axis is (1,0,0)
};
#endif
