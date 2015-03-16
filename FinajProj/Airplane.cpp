#include "airplane.h"

using namespace std;

Airplane::Airplane()
{
}


Airplane::~Airplane()
{
}

void Airplane::drawPlane(){
	glBegin(GL_POINTS);

	//glColor3f(1, 0, 0);
	glPointSize(100);

	for (int i = 0; i < 10000; i++)
	{
		glVertex3f(0, i, 0);
	}
	glEnd();
}