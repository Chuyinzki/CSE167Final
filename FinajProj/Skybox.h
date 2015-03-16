//code found here:https://github.com/BenFradet/LO12
#ifndef SKYBOX_H
#define SKYBOX_H
#include "Bitmap.h"
using namespace std;
class Skybox
{
public:
	GLuint* tabTextures;
	Bitmap* textures;
	int LoadTextures();
	void Draw(float x, float y, float z, float width, float height, float length);
};
#endif