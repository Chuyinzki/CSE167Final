#ifndef _OBJPARSER_H_
#define _OBJPARSER_H_
#include <string>
#include <vector>
#include "Matrix4.h"
#include "Vector3.h"
using namespace std;

class OBJParser
{
public:
	OBJParser();
	~OBJParser();
	double minX;
	double minY;
	double minZ;
	double maxX;
	double maxY;
	double maxZ;
	void displayParsedFile();
	Vector3 getCenter();
	Matrix4 getScale(double angle, double zDistance);
	void printBounds();
	enum objTypeEnum{ defaultERROR, VERTEX, VERTEX_NORMAL, FACE_VERTEX, FACE_VERTEX_COORDINATE, FACE_VERTEX_NORMAL, FACE_VERTEX_COORDINATE_NORMAL } objType;
	struct line //the line of text within the obj file.
	{
		objTypeEnum objType = defaultERROR;
		double vertexPosX = NULL;
		double vertexPosY = NULL;
		double vertexPosZ = NULL;
		double colorR = NULL;
		double colorG = NULL;
		double colorB = NULL;
		double vertexNormalX = NULL;
		double vertexNormalY = NULL;
		double vertexNormalZ = NULL;
		int vertexIndex0 = NULL;
		int vertexIndex1 = NULL;
		int vertexIndex2 = NULL;
		double coordinateIndex0 = NULL;
		double coordinateIndex1 = NULL;
		double coordinateIndex2 = NULL;
		double normalIndex0 = NULL;
		double normalIndex1 = NULL;
		double normalIndex2 = NULL;
	};
	vector<line> objVector;
	void parseFile(string fileName);
	void setNormals();
private:

	void resetBounds();
	void recalculateBounds(line l);
	void normalizeStruct();
	void setColors();
	void centerTheCloud();

};
#endif
