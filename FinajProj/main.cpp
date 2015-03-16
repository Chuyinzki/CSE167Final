#define _CRT_SECURE_NO_WARNINGS //stupid fopen bug.
#include "GLee.h"
#include "Skybox.h"
#include "Camera.h"
#include "Airplane.h"
#include "bezier.h"
#include "Model_OBJ.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <iostream>
#include <time.h>       /* time, for randomization */
#include <windows.h>
#include <mmsystem.h>

#include "stdafx.h"
#include "ThreeMaxLoader.h"
#include "OBJParser.h"
using namespace std;

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glut32.lib")

OBJParser objParser;

Model_OBJ obj;
Camera cam;
GLuint shader_program_1;
GLuint shader_program_2;
//#define SIZE 1024
const float OS = 10.0;
const float mp = OS / 3; //middle points(positive side)
const int QUAD_COUNT = 24;
bool toggleEnv = true;
GLfloat texpts[2][2][2] = {
	{ { 0, 0 }, { 1, 0 } }, { { 0, 1 }, { 1, 1 } }
};
Skybox skybox;
int timer = 0;
bool bezToggle = false;
bool thirdPersonView = false;

// window size
int ww, wh;

//shaders:
GLuint vs, fs, vs2, fs2;

GLint viewport[4];
GLdouble modelViewMatrix[16], projectionMatrix[16];

const int LENGTH = 513; //2^n+1
float m_heightmap[LENGTH][LENGTH];
float tempHeightMap[LENGTH][LENGTH]; //a copy of m_heightmap for using as temporary variables for blurring.
const float STARTING_HEIGHT = 0;
const float MAX_STARTING_NOISE = 8000;
const int OFFSET = 10000; //distance of terrain from origin in x,z axis

//Initializations for the Airplane
Airplane plane;

Vector4 p0(500, 7000, 0, 1);
Vector4 p1(-5000, 5000, 10000, 1);
Vector4 p2(-5000, 9000, -10000, 1);
Vector4 p3(500, 7000, 0, 1);

Vector4 p4(500, 7000, 0, 1);
Vector4 p5(5000, 9000, 10000, 1);
Vector4 p6(5000, 5000, -10000, 1);
Vector4 p7(500, 7000, 0, 1);

bezier bez(p0, p1, p2, p3, p4, p5, p6, p7);

//Initializations for the Portal
Vector4 p8(500, 6000, 300, 1);
Vector4 p9(500, 10000, -600, 1);
Vector4 p10(500, 10000, 1200, 1);
Vector4 p11(500, 6000, 300, 1);

bezier portal(p8, p9, p10, p11);

// define look-at positioning
static GLdouble eye[] = { 0, 3 * OFFSET, 0};
static GLdouble at[] = { 0.0, 0, 0 };

static double minHeight = MAX_STARTING_NOISE, maxHeight = -MAX_STARTING_NOISE;

GLdouble theta = atan(eye[1] / eye[2]) * 180 / 3.1415926;
GLdouble phi = 270.0;
GLdouble radius = sqrt(eye[1] * eye[1] + eye[2] * eye[2]);

// find how many hits when selection
int pointHits = 0;

// remember the moving control name
int MOVENAME = 0;

int patchMode = 2; // 1 - GL_LINE;
// 2 - GL_FILL;
// 3 - GL_POINT;

int typeMode = 1; // 1 - Bezier;
// 2 - Interpolating Cubic;

//store vertex positions and normals, so we don't recompute them every run:
struct vertexPosition{
	Vector3 position;
};

struct normalsAndPositions{
	float xNorm, yNorm, zNorm;
	float v1x, v2x, v3x, v1y, v2y, v3y, v1z, v2z, v3z;
};
vertexPosition vertexPosArray[LENGTH][LENGTH];
const int VERTEX_COUNT = (LENGTH - 1) * 2 * (LENGTH - 1);
normalsAndPositions vertexNormals[VERTEX_COUNT]; //size is how many normals are in the stacked triangles

double fps = 0;
int frameCount = 0;
int previousTime = 0;

void displayFPS()
{
	//  Increase frame count
	frameCount++;

	//  Get the number of milliseconds since glutInit called
	//  (or first call to glutGet(GLUT ELAPSED TIME)).
	int currentTime = glutGet(GLUT_ELAPSED_TIME);

	//  Calculate time passed
	int timeInterval = currentTime - previousTime;

	if (timeInterval > 1000)
	{
		//  calculate the number of frames per second
		fps = frameCount / (timeInterval / 1000.0f);

		//  Set time
		previousTime = currentTime;

		//  Reset frame count
		frameCount = 0;
	}
	//cout << "FPS: " << fps << endl;
}

void blurHeightmap(unsigned int passes) //blurs the heightmap (obviously). This removes sharp/steep areas in the terrain by rounding them with nearest neighbors.
{
	for (int pass = 0; pass < passes; pass++)
	{
		//for some reason creating the tempHeightMap array here was causing stack overflow...this function is only called once.
		//cout << "A" << endl;
		for (int i = 0; i < LENGTH; i++)
		for (int j = 0; j < LENGTH; j++)
			tempHeightMap[i][j] = m_heightmap[i][j];
		//cout << "B" << endl;
		for (int y = 1; y < LENGTH - 1; y++)
		{
			for (int x = 1; x < LENGTH - 1; x++)
			{
				float sumNeighbors = 0;
				for (int i = -1; i < 2; i++)
				for (int j = -1; j < 2; j++)
					sumNeighbors += tempHeightMap[y + i][x + j];
				m_heightmap[y][x] = sumNeighbors / 9;
			}
		}

		//now blur the perimiter of the heightmap
		//top:
		for (int i = 0; i < LENGTH; i++) {
			if (i == 0){
				//top left:
				m_heightmap[0][0] = (tempHeightMap[0][1] + tempHeightMap[1][1] + tempHeightMap[1][0] + tempHeightMap[0][0]) / 4.0f;
			}
			else if (i == LENGTH - 1) {
				//top right:
				m_heightmap[0][i] = (tempHeightMap[0][i - 1] + tempHeightMap[1][i - 1] + tempHeightMap[1][i - 1] + tempHeightMap[0][i]) / 4.0f;
				//bottom right:
				m_heightmap[i][i] = (tempHeightMap[i-1][i] + tempHeightMap[i][i-1] + tempHeightMap[i-1][i-1] + tempHeightMap[i][i]) / 4.0f;
				//bottom left:
				m_heightmap[i][0] = (tempHeightMap[i - 1][0] + tempHeightMap[i - 1][1] + tempHeightMap[i - 1][1] + tempHeightMap[i][0]) / 4.0f;
			}
			else{
				//top row:
				//cout << LENGTH - 1 << endl;
				m_heightmap[0][i] = (tempHeightMap[0][i - 1] + tempHeightMap[0][i + 1] + tempHeightMap[1][i - 1] + tempHeightMap[1][i] + tempHeightMap[1][i + 1] + tempHeightMap[0][i]) / 6.0f;
				//bottom row:
				m_heightmap[LENGTH-1][i] = (tempHeightMap[LENGTH-1][i - 1] + tempHeightMap[LENGTH-1][i + 1] + tempHeightMap[LENGTH-2][i - 1] + tempHeightMap[LENGTH - 2][i] + tempHeightMap[LENGTH - 2][i + 1] + tempHeightMap[LENGTH-1][i]) / 6.0f;
				//left row:
				m_heightmap[i][0] = (tempHeightMap[i - 1][0] + tempHeightMap[i + 1][0] + tempHeightMap[i - 1][1] + tempHeightMap[i][1] + tempHeightMap[i + 1][1] + tempHeightMap[i][0]) / 6.0f;
				//right row:
				m_heightmap[i][LENGTH-1] = (tempHeightMap[i - 1][LENGTH-1] + tempHeightMap[i + 1][LENGTH-1] + tempHeightMap[i - 1][LENGTH - 2] + tempHeightMap[i][LENGTH - 2] + tempHeightMap[i + 1][LENGTH - 2] + tempHeightMap[i][LENGTH-1]) / 6.0f;
			}
		}
	}
}

void setUniformsForShaders();
//do this after filling the vertexNormals array.
void setMaxAndMinHeight()
{
	minHeight = MAX_STARTING_NOISE; maxHeight = -MAX_STARTING_NOISE;
	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		if (vertexNormals[i].v1y < minHeight) minHeight = vertexNormals[i].v1y;
		if (vertexNormals[i].v1y > maxHeight) maxHeight = vertexNormals[i].v1y;

		if (vertexNormals[i].v2y < minHeight) minHeight = vertexNormals[i].v2y;
		if (vertexNormals[i].v2y > maxHeight) maxHeight = vertexNormals[i].v2y;

		if (vertexNormals[i].v3y < minHeight) minHeight = vertexNormals[i].v3y;
		if (vertexNormals[i].v3y > maxHeight) maxHeight = vertexNormals[i].v3y;
	}
	setUniformsForShaders();
}

//i could optimize this later, but it's only ran once in init, so I don't think it's a big deal.
void storeNormals() //only do this once. (init)
{
	float minX = -OFFSET;
	float maxX = OFFSET;
	float minZ = minX;
	float maxZ = maxX;
	int nTrianglesPerSide = LENGTH;
	double xRange = maxX - minX;
	//double yRange = maxY - minY;
	double zRange = maxZ - minZ;
	double triLenX = xRange / nTrianglesPerSide;
	//double triLenY = yRange / nTrianglesPerSide;
	double triLenZ = zRange / nTrianglesPerSide;
	//glFrontFace(GL_CW);
	double currX, currY, currZ;
	currZ = maxZ;
	Vector3 prev1; //vertex created 1 vertex ago.
	Vector3 prev2; //vertex created 2 vertices ago.
	Vector3 currVertex; //obvious
	Vector3 currNormal; //calculated with current vertice and 2 previous vertices to get normal of the triangle face.
	int i = 0; //index in normal array.
	for (int z = LENGTH - 1; z > 0; z--)
	{
		currX = minX;
		for (int x = 0; x < LENGTH - 1; x++)
		{
			if (x == 0)
			{
				currY = m_heightmap[z][x];
				prev2 = Vector3(currX, currY, currZ);
				//glNormal3f(0, 1, 0);
				currY = m_heightmap[z - 1][x];
				prev1 = Vector3(currX, currY, currZ - triLenZ);
			}
			else
			{
				prev2 = prev1;
				prev1 = currVertex;
			}
			currY = m_heightmap[z][x + 1];
			currVertex = Vector3(currX + triLenX, currY, currZ);
			currNormal.cross((currVertex-prev2), (prev1-prev2));
			currNormal.normalize();

			//store data:
			vertexNormals[i].xNorm = currNormal.getX();
			vertexNormals[i].yNorm = currNormal.getY();
			vertexNormals[i].zNorm = currNormal.getZ();
			vertexNormals[i].v1x = prev2.getX();
			vertexNormals[i].v1y = prev2.getY();
			vertexNormals[i].v1z = prev2.getZ();
			vertexNormals[i].v2x = currVertex.getX();
			vertexNormals[i].v2y = currVertex.getY();
			vertexNormals[i].v2z = currVertex.getZ();
			vertexNormals[i].v3x = prev1.getX();
			vertexNormals[i].v3y = prev1.getY();
			vertexNormals[i].v3z = prev1.getZ();
			i++;

			prev2 = prev1;
			prev1 = currVertex;
			currY = m_heightmap[z - 1][x + 1];
			currVertex = Vector3(currX + triLenX, currY, currZ - triLenZ);
			//prev1.print("p1");
			//prev2.print("p2");
			//currVertex.print("cv");
			currNormal.cross((prev2 - currVertex), (prev1 - currVertex));
			//currNormal.print("cn");
			currNormal.normalize();

			vertexNormals[i].xNorm = currNormal.getX();
			vertexNormals[i].yNorm = currNormal.getY();
			vertexNormals[i].zNorm = currNormal.getZ();
			vertexNormals[i].v1x = prev2.getX();
			vertexNormals[i].v1y = prev2.getY();
			vertexNormals[i].v1z = prev2.getZ();
			vertexNormals[i].v2x = prev1.getX();
			vertexNormals[i].v2y = prev1.getY();
			vertexNormals[i].v2z = prev1.getZ();
			vertexNormals[i].v3x = currVertex.getX();
			vertexNormals[i].v3y = currVertex.getY();
			vertexNormals[i].v3z = currVertex.getZ();
			i++;

			currX += triLenX;
		}
		currZ -= triLenZ;
	}
	setMaxAndMinHeight();
}

void printTriangle(int i) //debug method
{
	cout << "XNorm:" << vertexNormals[i].xNorm << endl;
	cout << "YNorm:" << vertexNormals[i].yNorm << endl;
	cout << "ZNorm:" << vertexNormals[i].zNorm << endl;
	cout << "v1x:" << vertexNormals[i].v1x << endl;
	cout << "v1y:" << vertexNormals[i].v1y << endl;
	cout << "v1z:" << vertexNormals[i].v1z << endl;
	cout << "v2x:" << vertexNormals[i].v2x << endl;
	cout << "v2y:" << vertexNormals[i].v2y << endl;
	cout << "v2z:" << vertexNormals[i].v2z << endl;
	cout << "v3x:" << vertexNormals[i].v3x << endl;
	cout << "v3y:" << vertexNormals[i].v3y << endl;
	cout << "v3z:" << vertexNormals[i].v3z << endl;
}

int negCount = 0;
int posCount = 0;

//randomize a float between -d and d
float RAND(float d)
{
	float retVal = -d + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2*d)));
	if (retVal < 0) negCount++; else posCount++;
	//cout << "RAND: " << " " << retVal << " " << negCount << " " << posCount << endl;
	return retVal;
}

void initCorners()
{
	m_heightmap[0][0] = 0;
	m_heightmap[0][LENGTH - 1] = 0;
	m_heightmap[LENGTH - 1][0] = 0;
	m_heightmap[LENGTH - 1][LENGTH - 1] = 0;

	//also get the first center:
	int x1 = 0;
	int x2 = LENGTH - 1;
	int y1 = x1;
	int y2 = x2;
	int centerX = (x2 - x1) / 2 + x1;
	int centerY = (y2 - y1) / 2 + y1;
	float a = m_heightmap[y1][x1];
	float b = m_heightmap[y1][x2];
	float c = m_heightmap[y2][x1];
	float d = m_heightmap[y2][x2];
	float e = (a + b + c + d) / 4 + RAND(MAX_STARTING_NOISE);
	m_heightmap[centerY][centerX] = e; //center of whole heightmap
}

//PRECONDITION: MAKE SURE THE TERRAIN IS A length OF 2^n+1
//this can probably be optimized, I think i re-calculate some points, but they will be correct.
void DiamondSquare(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, float maxNoise, int level) //first func call should have level 0, rest are slaves(level 1)
{
	if (x2 - x1 <= 1) return;
	//calculateCenter:
	//cout << x1 << " " << x2 << " " << y1 << " " << y2 << " " << endl;
	int centerX = (x2 - x1) / 2 + x1;
	int centerY = (y2 - y1) / 2 + y1;
	float a = m_heightmap[y1][x1];
	float b = m_heightmap[y1][x2];
	float c = m_heightmap[y2][x2];
	float d = m_heightmap[y2][x1];
	float e = m_heightmap[centerY][centerX]; //calculated in previous step or init.

	float f; //left center of current square. (left diamond point)
	float g; //top center of current square. (top diamond point)
	float h; //right center of current square. (right diamond point)
	float i; //bottom center of current square. (bottom diamond point)

	//float e = (a + b + c + d) / 4 + RAND(maxNoise);
	//m_heightmap[centerY][centerX] = e;

	//get 4 inside squares centers:
	maxNoise = maxNoise * 0.9f; //reduce allowable noise for side centers.
	
	//DIAMONDS:
	//top center:
	int halfDist = centerX - x1; //it is same for centerY-y1;
	//cout << "HD:" << halfDist << endl;
	if(y1 == 0) g = (a + b + e )/3 + RAND(maxNoise);
	else {
		float aboveTop = m_heightmap[y1-halfDist][centerX];
		g = (a + b + e + aboveTop) / 4 + RAND(maxNoise);
	}
	m_heightmap[y1][centerX] = g; //defined above

	//left center:
	if (x1 == 0) f = (a + d + e) / 3 + RAND(maxNoise);
	else {
		float leftOfLeft = m_heightmap[centerY][x1-halfDist];
		f = (a + d + e + leftOfLeft) / 4 + RAND(maxNoise);
	}
	m_heightmap[centerY][x1] = f; //defined above

	//right center:
	if (x2 == LENGTH-1) h = (b + c + e) / 3 + RAND(maxNoise);
	else{
		float rightOfRight = m_heightmap[centerY][x2 + halfDist];
		h = (b + d + e + rightOfRight) / 4 + RAND(maxNoise);
	}
	m_heightmap[centerY][x2] = h; //defined above

	//bottom center:
	if (y2 == LENGTH-1) i = (c + d + e) / 3 + RAND(maxNoise);
	else{
		float belowBottom = m_heightmap[y2 + halfDist][centerX];
		i = (c + d + e + belowBottom) / 4 + RAND(maxNoise);
	}
	m_heightmap[y2][centerX] = i; //defined above

	//SQUARES:
	if (centerX - x1 > 1){//there are more square steps to caclulate:
		int quarterDist = halfDist / 2;
		m_heightmap[centerY - quarterDist][centerX - quarterDist] = (a + g + e + f) / 4; //top left square's center
		m_heightmap[centerY - quarterDist][centerX + quarterDist] = (g + b + h + e) / 4; //top right square's center
		m_heightmap[centerY + quarterDist][centerX + quarterDist] = (e + h + c + i) / 4; //bottom right square's center
		m_heightmap[centerY + quarterDist][centerX - quarterDist] = (f + e + i + d) / 4; //bottom left square's center
	}

	bool hasChildren = false;
	int squareLen = x2 - x1;
	if (squareLen/2 > 1) hasChildren = true;
	int xStart = x1;
	int yStart = y1;
	int inc = squareLen; //distance between each sub square. recalculated immediately
	//do a BFS to fill the diamonds and squares, one level at a time.
	if (level == 1) return; //only the first function call will recursively call other functions.
	while (true) //iterate over all inside boxes for each different size. size reduces for each iteration through this loop.
	{
		inc = inc/2;
		//cout << "inc" << " " << inc << endl;
		if (inc <= 1) break;
		yStart = y1;
		while (yStart < LENGTH - 1){ //iterate over y
			xStart = x1;
			while (xStart < LENGTH - 1){ //iterate over x
				DiamondSquare(xStart, yStart, xStart+inc, yStart + inc, maxNoise, 1);
				xStart += inc;
			}
			yStart += inc;
		}
		maxNoise = maxNoise * 0.9f; //reduce allowable noise for side centers.
	}
}

Vector3 planeDir;
void renderAirplane(int i)
{
	int scaler = 2;
	//glClearColor(0.0f, 0.1f, 0.0f, 0.5f);
	glMatrixMode(GL_MODELVIEW);
	//obj.Draw();
	glPushMatrix();
	Vector4 theVector =  bez.vecy[i];
	Vector4 nextVector = bez.vecy[i + 1];
	Vector3 tv(theVector.get(0), theVector.get(1), theVector.get(2));
	Vector3 nv(nextVector.get(0), nextVector.get(1), nextVector.get(2));
	planeDir = nv - tv;
	//diff.print("diff");
	Vector3 start(0, 0, -1);
	planeDir.normalize();
	double result = planeDir.dot(planeDir, start);
	double xDiffAngle = result/ M_PI*180;
	cout << xDiffAngle << endl;
	//double yDiffAngle = 0 / 180.0 * M_PI;
	//DO TRANSLATIONS,SCALES,ROTATIONS OF AIRPLANE/JET HERE.
		glScalef(scaler, scaler, scaler);
		glTranslatef(theVector.get(0)/scaler, theVector.get(1)/scaler, theVector.get(2)/scaler);
		//glRotatef(xDiffAngle, 1,0, 0);
		glRotatef(xDiffAngle, 0, 1, 0);
		//cout << xDiffAngle << " " << yDiffAngle << endl;
		//glRotatef(yDiffAngle / scaler, 0, 1, 0);
		


		
		float no_mat[3] = { 0, 0, 0 };
		float grey[3] = { 0.4, 0.4, 0.4 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, grey);
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
		objParser.displayParsedFile();
	glPopMatrix();
}
//creates a grid of triangle strips with height taken from m_heightmap.
void createTerrain()
{
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		//glColor3f(0, 0, 0);
		glNormal3f(vertexNormals[i].xNorm, vertexNormals[i].yNorm, vertexNormals[i].zNorm);
		glVertex3f(vertexNormals[i].v1x, vertexNormals[i].v1y, vertexNormals[i].v1z);
		glVertex3f(vertexNormals[i].v2x, vertexNormals[i].v2y, vertexNormals[i].v2z);
		glVertex3f(vertexNormals[i].v3x, vertexNormals[i].v3y, vertexNormals[i].v3z);
		//printTriangle(i);
	}
	glEnd();
}

//SHADER LOADING CODE: from http://en.wikibooks.org/wiki/OpenGLshader_program_1ming/Modern_OpenGL_Tutorial_02
char* file_read(const char* filename)
{
	FILE* input = fopen(filename, "rb");
	if (input == NULL) return NULL;

	if (fseek(input, 0, SEEK_END) == -1) return NULL;
	long size = ftell(input);
	if (size == -1) return NULL;
	if (fseek(input, 0, SEEK_SET) == -1) return NULL;

	/*if using c-compiler: dont cast malloc's return value*/
	char *content = (char*)malloc((size_t)size + 1);
	if (content == NULL) return NULL;

	fread(content, 1, (size_t)size, input);
	if (ferror(input)) {
		free(content);
		return NULL;
	}

	fclose(input);
	content[size] = '\0';
	return content;
}

/**
* Display compilation errors from the OpenGL shader compiler
*/
void print_log(GLuint object)
{
	GLint log_length = 0;
	if (glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else if (glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else {
		fprintf(stderr, "printlog: Not a shader or a program\n");
		return;
	}

	char* log = (char*)malloc(log_length);

	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);

	fprintf(stderr, "%s", log);
	free(log);
}

/**
* Compile the shader from file 'filename', with error handling
*/

void drawPortal(){
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(500, 7000, 0);
	glutSolidCone(1000,1,20,20);

	glPopMatrix();
}

GLuint create_shader(const char* filename, GLenum type)
{
	const GLchar* source = file_read(filename);
	if (source == NULL) {
		fprintf(stderr, "Error opening %s: ", filename); perror("");
		return 0;
	}
	GLuint res = glCreateShader(type);
	const GLchar* sources[2] = {
#ifdef GL_ES_VERSION_2_0
		"#version 100\n"
		"#define GLES2\n",
#else
		"#version 120\n",
#endif
		source };
	glShaderSource(res, 2, sources, NULL);
	free((void*)source);

	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		fprintf(stderr, "%s:", filename);
		print_log(res);
		glDeleteShader(res);
		return 0;
	}

	return res;
}


void myDisplay()
{
	glClearColor(0, 0, 0.5, 1);
	if (timer >= bez.vecy.size() - 1) timer = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (timer < bez.vecy.size()/2) 
		glUseProgram(shader_program_1);
	else glUseProgram(shader_program_2);
	setUniformsForShaders();
	createTerrain();
	glUseProgram(0);
	renderAirplane(timer);
	drawPortal();
	//portal.draw();
	if (bezToggle == true) bez.draw();
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glutSwapBuffers();
	timer++;
	glutPostRedisplay();
}

// set up the position of a camera
void setupLookAtPosition()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//up = (right) cross (-eye)
	//eyeVec.print("eyePos");
	double offset1 = 1000;
	int offset2 = 20;


	if (!thirdPersonView){
		Vector3 upVec, rightVec(1, 0, 0), eyeVec(eye[0], eye[1], eye[2]);
		upVec.cross(rightVec, eyeVec*(-1));

		gluLookAt(eye[0], eye[1], eye[2],
			at[0], at[1], at[2],
			upVec.getX(), upVec.getY(), upVec.getZ());
	}
	else{
		Vector3 upVec, eyeVec(bez.vecy[timer].get(0), bez.vecy[timer].get(1), bez.vecy[timer].get(2));
		Vector3 rightVec;
		Vector3 yVec(0, 1, 0);
		rightVec.cross(planeDir, yVec);
		upVec.cross(rightVec, eyeVec*(-1));
		gluLookAt(bez.vecy[(timer - offset2) % bez.vecy.size()].get(0), bez.vecy[(timer - offset2) % bez.vecy.size()].get(1) + offset1, bez.vecy[(timer - offset2) % bez.vecy.size()].get(2),
			bez.vecy[(timer + 1) % bez.vecy.size()].get(0), bez.vecy[(timer + 1) % bez.vecy.size()].get(1), bez.vecy[(timer + 1) % bez.vecy.size()].get(2),
			upVec.getX(), upVec.getY(), upVec.getZ());
	}


	//upVec.print("up");
}

void idleFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	displayFPS();
	setupLookAtPosition();
	//moveSpheres();
	//glutPostRedisplay(); cout << "A" << endl;
	//myDisplay();
	//plane.drawPlane();
	//glutSwapBuffers();
}



void setUniformsForShaders()
{
	////set textures:
	//GLint XN = glGetUniformLocation(shader_program_1, "XN"); //0
	//GLint XP = glGetUniformLocation(shader_program_1, "XP"); //1
	//GLint YN = glGetUniformLocation(shader_program_1, "YN"); //2
	//GLint YP = glGetUniformLocation(shader_program_1, "YP"); //3
	//GLint ZN = glGetUniformLocation(shader_program_1, "ZN"); //4 
	//GLint ZP = glGetUniformLocation(shader_program_1, "ZP"); //5
	////cout << XN << XP << YN << YP << ZN << ZP << endl;
	//glUniform1i(XN, 0);
	//glUniform1i(XP, 1);
	//glUniform1i(YN, 2);
	//glUniform1i(YP, 3);
	//glUniform1i(ZN, 4);
	//glUniform1i(ZP, 5);

	////set offset:
	//GLint glslOS = glGetUniformLocation(shader_program_1, "offset");
	//glUniform1f(glslOS, OS);

	////eye:
	//GLfloat glslEye = glGetUniformLocation(shader_program_1, "eye");
	//glUniform3f(glslEye, eye[0], eye[1], eye[2]);
	////cout << "eye.y:" << eye[1] << endl;

	////cout << XN << XP << glslEye << endl;

	//heights:
	GLfloat uniMinHeight = glGetUniformLocation(shader_program_1, "minHeight");
	glUniform1f(uniMinHeight, minHeight);
	GLfloat uniMaxHeight = glGetUniformLocation(shader_program_1, "maxHeight");
	glUniform1f(uniMaxHeight, maxHeight);

	GLfloat uniMinHeight2 = glGetUniformLocation(shader_program_2, "minHeight");
	glUniform1f(uniMinHeight2, minHeight);
	GLfloat uniMaxHeight2 = glGetUniformLocation(shader_program_2, "maxHeight");
	glUniform1f(uniMaxHeight2, maxHeight);
	//cout << "max:" << maxHeight << endl;
}

int loadShaders()
{
	//load shaders:
	shader_program_1 = glCreateProgram();
	if ((vs = create_shader("shaders/terrain.vs", GL_VERTEX_SHADER)) == 0) return 0;
	if ((fs = create_shader("shaders/terrain.fs", GL_FRAGMENT_SHADER)) == 0) return 0;

	GLint link_ok = GL_FALSE;
	glAttachShader(shader_program_1, vs);
	glAttachShader(shader_program_1, fs);
	glLinkProgram(shader_program_1);
	glGetProgramiv(shader_program_1, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		print_log(shader_program_1);
		return 0;
	}

	return 1;
}

int loadShaders2()
{
	//load shaders:
	shader_program_2 = glCreateProgram();
	if ((vs2 = create_shader("shaders/toon2.vs", GL_VERTEX_SHADER)) == 0) return 0;
	if ((fs2 = create_shader("shaders/toon2.fs", GL_FRAGMENT_SHADER)) == 0) return 0;

	GLint link_ok = GL_FALSE;
	glAttachShader(shader_program_2, vs2);
	glAttachShader(shader_program_2, fs2);
	glLinkProgram(shader_program_2);
	glGetProgramiv(shader_program_2, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		print_log(shader_program_2);
		return 0;
	}

	return 1;
}

void setupProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( /* field of view in degree */ 40.0,
		/* aspect ratio */ 2.0,
		/* Z near */ 1.0, /* Z far */ 100000.0);
}

// initiate the environment before the program starts
GLfloat ambient[] = { 1, 1, 1, 1.0 };
GLfloat position[] = { 0, 1.0, 0, 0 };
GLfloat mat_diffuse[] = { 1, 1, 1, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
GLfloat red_light[] = { 1, 0, 0, 1 };
GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat no_mat[] = { 0, 0, 0, 1 };

void init()
{
	loadShaders();
	loadShaders2();
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	//obj file stuff:
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	setupProjectionMatrix();
	setupLookAtPosition();
	
	//set up Light0:
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, mat_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_light);
	glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
	srand(time(NULL)); //initialize random seed for the randomizer. This will cause a different terrain to be created every run.
	int start = rand(); //do this because before, the first rand was always positive.
	initCorners();
	const int MASTERLOOP = 0;
	DiamondSquare(0, 0, LENGTH - 1, LENGTH - 1, MAX_STARTING_NOISE, MASTERLOOP); //calculate and store heights into m_heightmap
	blurHeightmap(500);
	storeNormals(); //store normals of heightMap into vertexNormals[] so we can access them directly, instead of calculating more than once.

	/*for (int i = 0; i < LENGTH; i++){

		for (int j = 0; j < LENGTH; j++){
			cout << m_heightmap[i][j] << " ";
		}
		cout << endl;
	}*/
	//int shaderRetVal = loadShaders();
	//cout << shaderRetVal << endl;
	//skybox.LoadTextures();
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	//glEnable(GL_MAP2_VERTEX_3);
	//glEnable(GL_AUTO_NORMAL); //worked for light reflectance.\
		glMatrixMode(GL_PROJECTION);
	//obj.Load("plane4.obj"); //load obj file
	objParser.parseFile("plane4.obj");
}

void drawBox(void)
{
	glBegin(GL_QUADS);
	glColor3f(0, 1, 0);
	glVertex3f(-100, 0, -100);
	glVertex3f(-100, 0, 100);
	glVertex3f(100, 0, 100);
	glVertex3f(100, 0, -100);
	glEnd();
}

// setup the  perpective viewing
void proj()
{
	if (ww <= wh)
		glFrustum(-3.5, 3.5, -3.5*(GLfloat)wh / (GLfloat)ww, 3.5*(GLfloat)wh / (GLfloat)ww, 1.8, 20.0);
	else
		glFrustum(-3.5*(GLfloat)ww / (GLfloat)wh, 3.5*(GLfloat)ww / (GLfloat)wh, -3.5, 3.5, 1.8, 20.0);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	ww = w;
	wh = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//proj();
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = ww;
	viewport[3] = wh;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void resetTerrain(){

}
//// mouse motion function
//void myMouseMove(int xPosition, int yPosition)
//{
//	GLdouble pointX, pointY, pointZ;
//
//	if (pointHits > 0) {
//		glGetIntegerv(GL_VIEWPORT, viewport);
//		glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
//		glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
//
//		gluUnProject((GLdouble)xPosition, (GLdouble)(wh - yPosition), (GLdouble)z,
//			modelViewMatrix, projectionMatrix, viewport,
//			&pointX, &pointY, &pointZ);
//
//		ctrlPoints[MOVENAME / 4][MOVENAME % 4 - 1][0] = pointX;
//		ctrlPoints[MOVENAME / 4][MOVENAME % 4 - 1][1] = pointY;
//		ctrlPoints[MOVENAME / 4][MOVENAME % 4 - 1][2] = pointZ;
//
//		glutPostRedisplay(); cout << "A" << endl;
//	}
//}

// keyboard function for key :"i", "I", "o", "O" 
void myKeyboard(unsigned char key, int x, int y)
{
	static int test = 0;
	if (key == 'o' || key == 'O' || key == 'i' || key == 'I') {
		if (key == 'o' || key == 'O') radius += 1000;
		if (key == 'i' || key == 'I') radius -= 1000;
		radius += test;
		eye[0] = radius * cos(theta * 3.14 / 180) * cos(phi * 3.14 / 180);
		eye[1] = radius * sin(theta * 3.14 / 180);
		eye[2] = -radius * cos(theta * 3.14 / 180) * sin(phi * 3.14 / 180);
		//cout << eye[1] << endl;
	}

	if (key == 'l' || key == 'L') bezToggle = !bezToggle;

	if (key == 'r' || key == 'R') init();

	if (key == 'c' || key == 'C') thirdPersonView = !thirdPersonView;



	if (key == 'b') //add blurring!!!!!!
	{
		blurHeightmap(50);
		storeNormals();
		cout << "b" << endl;
		setupLookAtPosition();
		glutPostRedisplay();
	}
	if (key == 27)  exit(0); //esc
	setupLookAtPosition();
	glutPostRedisplay();
}

// spectial key function for up arrow, down arrow, left arrow and right arrow
void specialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_DOWN:
		theta -= 1;
		break;
	case GLUT_KEY_UP:
		theta += 1;
		break;
	case GLUT_KEY_RIGHT:
		phi -= 1;
		break;
	case GLUT_KEY_LEFT:
		phi += 1;
		break;
	case GLUT_KEY_PAGE_UP:		// turn and move camera up 
		radius += 0.01;
		break;
	case GLUT_KEY_PAGE_DOWN:	// turn and move camera down
		radius -= 0.01;
		break;
	}
	eye[0] = radius * cos(theta * 3.14 / 180) * cos(phi * 3.14 / 180);
	eye[1] = radius * sin(theta * 3.14 / 180);
	eye[2] = -radius * cos(theta * 3.14 / 180) * sin(phi * 3.14 / 180);
	setupLookAtPosition();
	glutPostRedisplay(); //cout << "A" << endl;
}

// main function
int main(int argc, char** argv)
{
	PlaySound((LPCWSTR)"imperial_march.mp3", NULL, SND_FILENAME | SND_ASYNC);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
	//glutFullScreen();
	//glutInitWindowSize(500, 500);
	//glutInitWindowPosition(50, 50);
	glutCreateWindow("Aviation to the Unknown");
	init();
	glutIdleFunc(idleFunc);
	//glutReshapeFunc(reshape);
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(specialKeys);

	//glutMouseFunc(myPick);
	//glutMotionFunc(myMouseMove);
	glutMainLoop();
	return 0;
}
