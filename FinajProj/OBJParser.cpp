#include "OBJParser.h"
#include <gl/glut.h>
OBJParser::OBJParser()
{
	objType = defaultERROR;
	resetBounds();
}


OBJParser::~OBJParser()
{
}

//stores points into objVector.
void OBJParser::parseFile(string fileName)
{
	resetBounds();
	objVector.clear(); //clear the vector so we don't concatinate file vectors.
	FILE * pFile;

	cout << "1" << fileName << endl;
	FILE* fp;     // file pointer
	float x, y, z;  // vertex coordinates
	float r, g, b;  // vertex color
	int c1, c2;    // characters read from file

	fp = fopen(fileName.c_str(), "rb");
	if (fp == NULL) { cerr << "error loading file" << endl; exit(-1); }
	c1 = fgetc(fp);
	c2 = fgetc(fp);
	while (c1 != EOF)
	{

		if (c1 == 'v' && c2 == ' ') //v # # #
		{
			line OBJLine;
			OBJLine.objType = VERTEX;
			//fscanf(fp, "%lf %lf %lf %lf %lf %lf\n", &OBJLine.vertexPosX, &OBJLine.vertexPosY, &OBJLine.vertexPosZ, &OBJLine.colorR, &OBJLine.colorG, &OBJLine.colorB);
			fscanf(fp, "%lf %lf %lf\n", &OBJLine.vertexPosX, &OBJLine.vertexPosY, &OBJLine.vertexPosZ);
			//cout << OBJLine.vertexPosY << endl;
			objVector.push_back(OBJLine);
			recalculateBounds(OBJLine);
		}
		else if (c1 == 'v' && c2 == 'n')//vp # # #
		{
			line OBJLine;
			OBJLine.objType = VERTEX_NORMAL;
			float normX;
			float normY;
			float normZ;
			fscanf(fp, " %f %f %f", &normX, &normY, &normZ);
			Vector3 norms(normX, normY, normZ);
			norms.normalize();
			OBJLine.vertexNormalX = norms.getX();
			OBJLine.vertexNormalY = norms.getY();
			OBJLine.vertexNormalZ = norms.getZ();
			objVector.push_back(OBJLine);
		}
		else if (c1 == 'f' && c2 == ' ') //f #//#
		{
			line OBJLine;
			OBJLine.objType = FACE_VERTEX_NORMAL;
			
			fscanf(fp, "%i/%i %i/%i %i/%i\n", &OBJLine.vertexIndex0, &OBJLine.normalIndex0, &OBJLine.vertexIndex1, &OBJLine.normalIndex1, &OBJLine.vertexIndex2, &OBJLine.normalIndex2);
			objVector.push_back(OBJLine);
			//cout << "faces:" << OBJLine.vertexIndex0 << " " << OBJLine.vertexIndex1 << " " << OBJLine.vertexIndex2 << " " << endl;
		}
		else if (c1 == '#')//comments in file
		{
			fscanf(fp, "%*[^\n]", NULL);
		}
		c1 = fgetc(fp);
		if (c1 != '\n' && c1 != '#')
			c2 = fgetc(fp);
	}
	fclose(fp);   // make sure you don't forget to close the file when done
	cout << "finished parsing OBJ file, vector size: " << objVector.size() << endl;
	centerTheCloud();
	setNormals();
}


void OBJParser::centerTheCloud()
{
	Vector3 center = getCenter();
	for (unsigned int i = 0; i < objVector.size(); i++)
	{
		if (objVector[i].objType == VERTEX)
		{
			objVector[i].vertexPosX = objVector[i].vertexPosX - center.getX();
			objVector[i].vertexPosY = objVector[i].vertexPosY - center.getY();
			objVector[i].vertexPosZ = objVector[i].vertexPosZ - center.getZ();
		}
	}
}


void OBJParser::resetBounds()
{
	minX = 100000;
	minY = 100000;
	minZ = 100000;
	maxX = -100000;
	maxY = -100000;
	maxZ = -100000;
}


void OBJParser::recalculateBounds(line l)
{
	if (l.vertexPosX > maxX){ maxX = l.vertexPosX; }
	if (l.vertexPosY > maxY){ maxY = l.vertexPosY; }
	if (l.vertexPosZ > maxZ){ maxZ = l.vertexPosZ; }
	if (l.vertexPosX < minX){ minX = l.vertexPosX; }
	if (l.vertexPosY < minY){ minY = l.vertexPosY; }
	if (l.vertexPosZ < minZ){ minZ = l.vertexPosZ; }
}

Vector3 OBJParser::getCenter()
{
	double midX = maxX - (maxX - minX) / 2;
	double midY = maxY - (maxY - minY) / 2;
	double midZ = maxZ - (maxZ - minZ) / 2;
	Vector3 retVec(midX, midY, midZ);
	return retVec;
}

Matrix4 OBJParser::getScale(double fovy, double zDistance)
{
	double cloudXLength = maxX - minX;
	double cloudYLength = maxY - minY;
	double tan30 = tan(fovy / 2.0 / 180.0*M_PI);
	double FrustLength = tan30 * 2 * abs(zDistance);
	double xScale = FrustLength / cloudXLength;
	double yScale = FrustLength / cloudYLength;
	double scaleChoice = (xScale < yScale) ? xScale : yScale;
	Matrix4 scaleMatrix;
	scaleMatrix.makeScale(scaleChoice, scaleChoice, scaleChoice);
	return scaleMatrix;
}

void OBJParser::setNormals()
{
	double p1xPos, p1yPos, p1zPos, p2xPos, p2yPos, p2zPos, p3xPos, p3yPos, p3zPos, vec1X, vec1Y, vec1Z, vec2X, vec2Y, vec2Z;
	for (unsigned int i = objVector.size() - 2; i >= 0; i--)
	{
		if (objVector[i].objType == FACE_VERTEX_NORMAL)
		{
			//cout << i << endl;
			p1xPos = objVector[objVector[i].vertexIndex0 - 1].vertexPosX;
			p1yPos = objVector[objVector[i].vertexIndex0 - 1].vertexPosY;
			p1zPos = objVector[objVector[i].vertexIndex0 - 1].vertexPosZ;

			p2xPos = objVector[objVector[i].vertexIndex1 - 1].vertexPosX;
			p2yPos = objVector[objVector[i].vertexIndex1 - 1].vertexPosY;
			p2zPos = objVector[objVector[i].vertexIndex1 - 1].vertexPosZ;

			p3xPos = objVector[objVector[i].vertexIndex2 - 1].vertexPosX;
			p3yPos = objVector[objVector[i].vertexIndex2 - 1].vertexPosY;
			p3zPos = objVector[objVector[i].vertexIndex2 - 1].vertexPosZ;

			vec1X = p2xPos - p1xPos;
			vec1Y = p2yPos - p1yPos;
			vec1Z = p2zPos - p1zPos;
			Vector3 vec1(vec1X, vec1Y, vec1Z);

			vec2X = p3xPos - p1xPos;
			vec2Y = p3yPos - p1yPos;
			vec2Z = p3zPos - p1zPos;
			Vector3 vec2(vec2X, vec2Y, vec2Z);

			Vector3 normal;
			normal.cross(vec1, vec2);
			normal.normalize();
			//normal.print("normal");
			objVector[objVector[i].vertexIndex0 - 1].vertexNormalX = normal.getX();
			objVector[objVector[i].vertexIndex0 - 1].vertexNormalY = normal.getY();
			objVector[objVector[i].vertexIndex0 - 1].vertexNormalZ = normal.getZ();

			objVector[objVector[i].vertexIndex1 - 1].vertexNormalX = normal.getX();
			objVector[objVector[i].vertexIndex1 - 1].vertexNormalY = normal.getY();
			objVector[objVector[i].vertexIndex1 - 1].vertexNormalZ = normal.getZ();

			objVector[objVector[i].vertexIndex2 - 1].vertexNormalX = normal.getX();
			objVector[objVector[i].vertexIndex2 - 1].vertexNormalY = normal.getY();
			objVector[objVector[i].vertexIndex2 - 1].vertexNormalZ = normal.getZ();
		}
		else
		{
			break; //stopped seeing faces.
		}
	}
}

//THIS MAY NEED TO BE OPTIMIZED TO USE MULTIPLE VECTORS OF STRUCTS INSTEAD OF 1.
void OBJParser::displayParsedFile()
{
	//glPointSize(8);
	//	double length = maxZ - minZ; //absolute difference of farthest point from closest point.
	//	double maxPointRadius = .001; //largest size of quad.
	//	double x, y, z, newRadius, shrinkage, halfLength, doubleLength;
	//	halfLength = length / 2;
	//	doubleLength = length * 2;
	glMatrixMode(GL_MODELVIEW);  // make sure we're in Modelview mode
	//glPointSize(8);
	//glBegin(GL_POINTS);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_POINT_SMOOTH);
	//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	//int objCount = 0;
	//cout << objVector.size() << endl;
	glBegin(GL_TRIANGLES);
	for (unsigned int i = objVector.size() - 2; i >= 0; i--)
	{
		if (objVector[i].objType == FACE_VERTEX_NORMAL)
		{
			//cout << "found" << endl;
			//int vertexIndex0 = objVector[i].vertexIndex0 - 1;
			//int vertexIndex1 = objVector[i].vertexIndex1 - 1;
			//int vertexIndex2 = objVector[i].vertexIndex2 - 1;
			//cout << vertexIndex0 << " " << vertexIndex1 << " " << vertexIndex2 << " " << endl;
			//cout << "HERE2" << endl;
			//if (objCount < 10) { cout << i << ": " << vertexIndex0 << " " << objCount << endl; objCount++; }
			//normal index = index
			//glBegin(GL_TRIANGLES);
			//glColor3f(objVector[i].r, objVector[i].g, objVector[i].b);
			//cout << objVector[objVector[i].vertexIndex0 - 1].vertexNormalY << endl;
			glNormal3d(objVector[objVector[i].vertexIndex0 - 1].vertexNormalX, objVector[objVector[i].vertexIndex0 - 1].vertexNormalY, objVector[objVector[i].vertexIndex0 - 1].vertexNormalZ);
			glVertex3d(objVector[objVector[i].vertexIndex0 - 1].vertexPosX, objVector[objVector[i].vertexIndex0 - 1].vertexPosY, objVector[objVector[i].vertexIndex0 - 1].vertexPosZ);

			glNormal3d(objVector[objVector[i].vertexIndex1 - 1].vertexNormalX, objVector[objVector[i].vertexIndex1 - 1].vertexNormalY, objVector[objVector[i].vertexIndex1 - 1].vertexNormalZ);
			glVertex3d(objVector[objVector[i].vertexIndex1 - 1].vertexPosX, objVector[objVector[i].vertexIndex1 - 1].vertexPosY, objVector[objVector[i].vertexIndex1 - 1].vertexPosZ);

			glNormal3d(objVector[objVector[i].vertexIndex2 - 1].vertexNormalX, objVector[objVector[i].vertexIndex2 - 1].vertexNormalY, objVector[objVector[i].vertexIndex2 - 1].vertexNormalZ);
			glVertex3d(objVector[objVector[i].vertexIndex2 - 1].vertexPosX, objVector[objVector[i].vertexIndex2 - 1].vertexPosY, objVector[objVector[i].vertexIndex2 - 1].vertexPosZ);
			//glEnd();
		}
		else
		{
			break; //stopped seeing faces.
		}
		//cout << "HERE" << endl;
	}
	glEnd();
}