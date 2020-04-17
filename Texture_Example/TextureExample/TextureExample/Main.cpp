#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>
#include "GL/glut.h"
#include "Main.h"
#include "GL/std_image.h"

using namespace std;
using std::vector;

GLfloat xRotated, yRotated, zRotated;
int w_image, h_image, nrChannels;
unsigned char* data_tex;
unsigned int texture;
int type;

void loadTextureFromFile()
{
	char name[100];

	cout << "Ingrese el nombre de la textura: ";
	cin >> name;
	cin >> type;

	data_tex = stbi_load(name, &w_image, &h_image, &nrChannels, 0);
	cout << "w_image: " << w_image << endl;
	cout << "h_image: " << h_image << endl;
	cout << "nrChannels: " << nrChannels << endl;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (data_tex) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w_image, h_image, 0, GL_RGB, GL_UNSIGNED_BYTE, data_tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else {
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data_tex);
}

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDepthMask(GL_TRUE);
	glLoadIdentity();

	double pi = 3.141592665885;
	int r = 1;
	double x, y, z;
	double u, v;
	int divisions = 50;
	double dTheta = 180 / divisions, dLon = 360 / divisions, degToRad = pi / 180;
	double height = 5.0;
	int faces = 180;

	if (type == 0) {
		glTranslatef(0.0, 0.0, -5);
		glRotatef(yRotated, 0, 1, 0);
		glRotatef(zRotated, 0, 0, 1);
		glRotatef(zRotated, 1, 0, 0);

		for (double lat = 0; lat <= 180; lat += dTheta) {
			glBegin(GL_QUAD_STRIP);
			for (double lon = 0; lon <= 360; lon += dLon) {
				//Vertex 1
				x = r * cos(lon * degToRad) * sin(lat * degToRad);
				y = r * sin(lon * degToRad) * sin(lat * degToRad);
				z = r * cos(lat * degToRad);
				glNormal3d(x, y, z);
				//u = atan2(x, z) / (2 * pi) + 0.5;
				//v = y * 0.5 + 0.5;
				glTexCoord2d(lon / 360, lat / 180);
				//glTexCoord2d(u, v);
				glVertex3d(x, y, z);
				//Vetex 2
				x = r * cos(lon * degToRad) * sin((lat + dTheta) * degToRad);
				y = r * sin(lon * degToRad) * sin((lat + dTheta) * degToRad);
				z = r * cos((lat + dTheta) * degToRad);
				glNormal3d(x, y, z);
				glTexCoord2d(lon / 360, (lat + dTheta) / (180));
				glVertex3d(x, y, z);
				//Vertex 3
				x = r * cos((lon + dLon) * degToRad) * sin(lat * degToRad);
				y = r * sin((lon + dLon) * degToRad) * sin(lat * degToRad);
				z = r * cos(lat * degToRad);
				glNormal3d(x, y, z);
				glTexCoord2d((lon + dLon) / 360, lat / 180);
				glVertex3d(x, y, z);
				//Vertex 4
				x = r * cos((lon + dLon) * degToRad) * sin((lat + dTheta) * degToRad);
				y = r * sin((lon + dLon) * degToRad) * sin((lat + dTheta) * degToRad);
				z = r * cos((lat + dTheta) * degToRad);
				glNormal3d(x, y, z);
				glTexCoord2d((lon + dLon) / 360, (lat + dTheta) / (180));
				glVertex3d(x, y, z);
			}
			glEnd();
			
		}
	} else {
		glTranslatef(0.0, -2.0, -10);
		glRotatef(yRotated, 0, 1, 0);
		glRotatef(zRotated, 0, 0, 1);
		glRotatef(zRotated, 1, 0, 0);

		y = height;

		glBegin(GL_QUAD_STRIP);

		for (int i = 0; i <= faces; i++) {
			double u = i / (double)faces;
			x = r * cos(2 * pi * u);
			z = r * sin(2 * pi * u);
			glTexCoord2d(u, 1.0); glVertex3d(x, 0, z);
			glTexCoord2d(u, 0.0); glVertex3d(x, y, z);
		}

		glEnd();
	}

	glFlush();
	glDisable(GL_TEXTURE_2D);

}

void resizeWindow(int x, int y)
{
	if (y == 0 || x == 0) return;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(40.0, (GLdouble)x / (GLdouble)y, 0.5, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, x, y);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

void idleFunc(void)
{

	yRotated += 0.1;
	zRotated += 0.1;
	drawScene();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(240, 240);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	loadTextureFromFile();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(resizeWindow);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idleFunc);
	glutMainLoop();
	return 0;
}
