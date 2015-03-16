//code found here:https://github.com/BenFradet/LO12
#include "Skybox.h"
int Skybox::LoadTextures()
{
	tabTextures = new GLuint[6];
	textures = new Bitmap[6];
	glGenTextures(6, tabTextures);
	textures[0].LoadBmp("data/texture/XN.bmp", 0); //left
	textures[1].LoadBmp("data/texture/XP.bmp", 0); //right
	textures[2].LoadBmp("data/texture/YN.bmp", 0); //bottom
	textures[3].LoadBmp("data/texture/YP.bmp", 0); //top
	textures[4].LoadBmp("data/texture/ZN.bmp", 0); //far
	textures[5].LoadBmp("data/texture/ZP.bmp", 0); //near
	for (int i = 0; i<6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, tabTextures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, textures[i].width, textures[i].height, 0, GL_RGB, GL_UNSIGNED_BYTE, textures[i].data);
	}
	return 0;
}
void Skybox::Draw(float x, float y, float z, float width, float height, float length)
{
	glEnable(GL_TEXTURE_2D);
	//make calculations easier by shifting center to most negative corner
	x = x - width / 2;
	y = y - height / 2;
	z = z - length / 2;

	//// Draw Front side
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, tabTextures[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z + length);
	glEnd();
	// Draw Back side
	glBindTexture(GL_TEXTURE_2D, tabTextures[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
	glEnd();

	// Draw Left side
	glBindTexture(GL_TEXTURE_2D, tabTextures[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z);
	glEnd();

	// Draw Right side
	glBindTexture(GL_TEXTURE_2D, tabTextures[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
	glEnd();

	// Draw Up side
	glBindTexture(GL_TEXTURE_2D, tabTextures[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z);
	glEnd();

	//// Draw Down side
	/*glBindTexture(GL_TEXTURE_2D, tabTextures[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y, z);
	glEnd();*/


	glDisable(GL_TEXTURE_2D);

}