#include "bezier.h"

long factorial(long n)
{
	long result = 1;

	//If n is 0, by definition 0! is equal to 1
	if (n <= 0)
		return result;

	//Calculate the factorial, n * n-1 * n-2 * ... * 1
	for (long i = n; i > 0; --i)
	{
		result *= i;
	}

	return result;
}

long combo(long n, long i)
{
	//C(n, i) = n! / ((n-1)! * i!)
	return factorial(n) / (factorial(n - i) * factorial(i));
}

double bernTwerkerCoeff(long n, long i, double t)
{
	//long n: is the degree of our curve, in the case of a cubic curve it is 3
	//long i: the index of the Bernstein coefficient and the control point
	//double t: is the time we are evaluating at

	//Calculate the Bernstein coefficient
	return combo(n, i) * pow(1.0 - t, n - i) * pow(t, i);
}

Vector4 curveCalc(double tt, Vector4 pd0, Vector4 pd1, Vector4 pd2, Vector4 pd3){
	//set points
	Vector4 pt0 = pd0;
	Vector4 pt1 = pd1;
	Vector4 pt2 = pd2;
	Vector4 pt3 = pd3;

	//Setup the control point matrix
	Matrix4 mP = Matrix4(pt0.get(0), pt1.get(0), pt2.get(0), pt3.get(0),
		pt0.get(1), pt1.get(1), pt2.get(1), pt3.get(1),
		pt0.get(2), pt1.get(2), pt2.get(2), pt3.get(2),
		0.0, 0.0, 0.0, 0.0);

	//Pick a time t
	double t = tt;

	//Create a vector with our Bernstein coefficients
	Vector4 C(bernTwerkerCoeff(3, 0, t),
		bernTwerkerCoeff(3, 1, t),
		bernTwerkerCoeff(3, 2, t),
		bernTwerkerCoeff(3, 3, t));

	//Calculate the final point q
	Vector4 q = mP * C;

	double x = q.get(0);
	double y = q.get(1);
	double z = q.get(2);

	//And make sure q is a point by setting its w-component to 1
	q.set(3, 1.0);

	return q;
}

bezier::bezier(Vector4 ps0, Vector4 ps1, Vector4 ps2, Vector4 ps3,
	Vector4 ps4, Vector4 ps5, Vector4 ps6, Vector4 ps7)
{
	p0 = ps0;
	p1 = ps1;
	p2 = ps2;
	p3 = ps3;

	p4 = ps4;
	p5 = ps5;
	p6 = ps6;
	p7 = ps7;

	init();
}

bezier::bezier(Vector4 ps0, Vector4 ps1, Vector4 ps2, Vector4 ps3)
{
	p0 = ps0;
	p1 = ps1;
	p2 = ps2;
	p3 = ps3;

	init2();
}

void bezier::init(){
	double delta = 0.005;

	Vector4 curve0;
	Vector4 curve1;

	for (double i = 0.0; i <= 1.0; i += delta){

		curve0 = curveCalc(i, p0, p1, p2, p3);
		vecy.push_back(curve0);
		//normy.push_back(normal);
	}

	for (double j = 0.0; j <= 1.0; j += delta){

		curve1 = curveCalc(j, p4, p5, p6, p7);
		vecy.push_back(curve1);
		//normy.push_back(normal);
	}
}

void bezier::init2(){
	double delta = 0.005;
	Vector4 curve0;

	for (double i = 0.0; i <= 1.0; i += delta){

		curve0 = curveCalc(i, p0, p1, p2, p3);
		vecy.push_back(curve0);
		//normy.push_back(normal);
	}
}

bezier::~bezier()
{
}


void bezier::draw(){
	//glClear(GL_COLOR_BUFFER_BIT);
	//glDisable(GL_CULL_FACE);
	double v1, v2, v3;

	float no_mat[3] = { 0, 0, 0 };
	float red[3] = { 1.0, 0, 0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
	glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
	
	//glColor3f(0, 1, 0);
	glBegin(GL_POINTS);

	for (int k = 0; k < vecy.size() - 1; k++){
			v1 = rand() % 100;
			v2 = rand() % 100;
			v3 = rand() % 100;
			glColor3f(v1/100.0, v2/100.0, v3/100.0);

			// Draw one point
			//glNormal3f(normy[k][l].get(0), normy[k][l].get(1), normy[k][l].get(2));
			glVertex3f(vecy[k].get(0), vecy[k].get(1), vecy[k].get(2));
		}

	glEnd();
	
}
