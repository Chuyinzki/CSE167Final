#pragma once
#include "Vector4.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "GL/glut.h"
#include <vector>
#include <cmath>

class bezier
{
public:
	bezier(Vector4, Vector4, Vector4, Vector4, Vector4, Vector4, Vector4, Vector4);
	bezier(Vector4, Vector4, Vector4, Vector4);
	~bezier();

	//Vector4 curveCalc(double tt, Vector4 p0, Vector4 p1, Vector4 p2, Vector4 p3);
	void draw();
	Vector3 getNormal(double t1, double t2);
	std::vector<Vector4> vecy;

private:
	/*Vector4 pt0, pt1, pt2, pt3;
	Matrix4 mP;*/
	void init();
	void init2();

	std::vector<Vector3> normy;

	Vector4 p0, p1, p2, p3,
		p4, p5, p6, p7;
};

