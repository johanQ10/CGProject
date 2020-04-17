#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <Math.h>
#include <fstream>
#include <string>
#include <vector>
#include "GL/glut.h"
#include "Main.h"
#include "Point.h"
#include "UserInterface.h"
#include "GL/std_image.h"

using namespace std;
using std::vector;

typedef enum { CULLING_BACK, CULLING_FRONT } Culling;
typedef enum { OFF, EYE_LINEAR, SPHERE_MAP, OBJECT_LINEAR} TexEnv;

Culling currentCulling;
TexEnv currentTexEnv;
char title[] = "Proyecto 3 - Johan Quintero";
int windowWidth, windowHeight;
int windowPosX, windowPosY;
int n, m, k, num;
float* mAmbientColor, * mDiffuseColor, * mSpecularColor, * lAmbientColor, * lDiffuseColor, * lSpecularColor;
int cursorX, cursorY, prevCursorX, prevCursorY;
float angleX, angleY, angleZ, rotateX, rotateY, rotateZ;
float translateX, translateY, translateZ;
float scaleX, scaleY, scaleZ;
bool leftPress, rightPress, middlePress;
bool xPress, yPress, zPress;
bool aPress, rPress, vPress, bPress;
bool unoPress, dosPress, tresPress, cuatroPress;
bool cincoPress, seisPress, sietePress, ochoPress;
bool zbuffer, cPress;
float nearClipping, farClipping, shininess;
vector <CPoint> vertex;
vector <CPoint> triangles;
vector <CPoint> normales;
vector <CPoint> centroide;
CUserInterface* userInterface;
CPoint pMin, pMax;
int w_image, h_image, nrChannels;
unsigned char* data_tex;
unsigned int texture;

bool load()
{
	char name[100];

	cout << "Ingrese el nombre del archivo: ";
	cin >> name;

	ifstream inFile;
	string off = "";
	char buffer[100];

	inFile.open(name);

	inFile >> off;

	if (off != "OFF") {
		cout << "No encontrado" << endl;
		return false;
	}
	else {

		inFile >> n >> m >> k;
		cout << n << " " << m << " " << k << endl;

		CPoint point;

		for (int i = 0; i < n; i++) {
			inFile >> point.x >> point.y >> point.z;
			vertex.push_back(point);
		}

		for (int i = 0; i < m; i++) {
			inFile >> num >> point.x >> point.y >> point.z;
			triangles.push_back(point);
		}

		inFile.close();

		return true;
	}
}

void loadTexture() {
	char name[100];

	cout << "Ingrese el nombre de la textura: ";
	cin >> name;

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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else {
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data_tex);
}

void initGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glShadeModel(GL_SMOOTH);

	for (int i = 0; i < m; i++) {

		pMin.x = MIN(pMin.x, vertex[triangles[i].x].x);
		pMin.x = MIN(pMin.x, vertex[triangles[i].y].x);
		pMin.x = MIN(pMin.x, vertex[triangles[i].z].x);

		pMin.y = MIN(pMin.y, vertex[triangles[i].x].y);
		pMin.y = MIN(pMin.y, vertex[triangles[i].y].y);
		pMin.y = MIN(pMin.y, vertex[triangles[i].z].y);

		pMin.z = MIN(pMin.z, vertex[triangles[i].x].z);
		pMin.z = MIN(pMin.z, vertex[triangles[i].y].z);
		pMin.z = MIN(pMin.z, vertex[triangles[i].z].z);

		pMax.x = MAX(pMax.x, vertex[triangles[i].x].x);
		pMax.x = MAX(pMax.x, vertex[triangles[i].y].x);
		pMax.x = MAX(pMax.x, vertex[triangles[i].z].x);

		pMax.y = MAX(pMax.y, vertex[triangles[i].x].y);
		pMax.y = MAX(pMax.y, vertex[triangles[i].y].y);
		pMax.y = MAX(pMax.y, vertex[triangles[i].z].y);

		pMax.z = MAX(pMax.z, vertex[triangles[i].x].z);
		pMax.z = MAX(pMax.z, vertex[triangles[i].y].z);
		pMax.z = MAX(pMax.z, vertex[triangles[i].z].z);

		CPoint P, Q, normal;

		P.x = vertex[triangles[i].z].x - vertex[triangles[i].x].x;
		P.y = vertex[triangles[i].z].y - vertex[triangles[i].x].y;
		P.z = vertex[triangles[i].z].z - vertex[triangles[i].x].z;

		Q.x = vertex[triangles[i].y].x - vertex[triangles[i].x].x;
		Q.y = vertex[triangles[i].y].y - vertex[triangles[i].x].y;
		Q.z = vertex[triangles[i].y].z - vertex[triangles[i].x].z;

		normal.x = P.y * Q.z - P.z * Q.y;
		normal.y = P.z * Q.x - P.x * Q.z;
		normal.z = P.x * Q.y - P.y * Q.x;

		GLfloat modulo = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		modulo = 1.0 / modulo;

		normal.x *= modulo;
		normal.y *= modulo;
		normal.z *= modulo;

		CPoint centro = CPoint((vertex[triangles[i].x].x + vertex[triangles[i].y].x + vertex[triangles[i].z].x) / 3,
			(vertex[triangles[i].x].y + vertex[triangles[i].y].y + vertex[triangles[i].z].y) / 3,
			(vertex[triangles[i].x].z + vertex[triangles[i].y].z + vertex[triangles[i].z].z) / 3);

		normales.push_back(normal);
		centroide.push_back(centro);
	}


}

void mouseButton(int button, int state, int x, int y) {
	if (TwEventMouseButtonGLUT(int(button), int(state), int(x), int(y)))
		return;

	prevCursorX = cursorX;
	prevCursorY = cursorY;
	cursorX = x;
	cursorY = y;

	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			leftPress = true;
		else leftPress = false;
		break;
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN)
			middlePress = true;
		else middlePress = false;
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
			rightPress = true;
		else rightPress = false;
		break;
	case 3:
		if (xPress) scaleX += 0.1;
		if (yPress) scaleY += 0.1;
		if (zPress) scaleZ += 0.1;
		break;
	case 4:
		if (xPress) scaleX -= 0.1;
		if (yPress) scaleY -= 0.1;
		if (zPress) scaleZ -= 0.1;
		break;
	default:
		cout << "Otro mouse" << endl;
		break;
	}
}

void keyInput(unsigned char key, int x, int y) {
	prevCursorX = cursorX;
	prevCursorY = cursorY;
	cursorX = x;
	cursorY = y;

	switch (key) {
	case 'x':
		xPress = true;
		break;
	case 'y':
		yPress = true;
		break;
	case 'z':
		zPress = true;
		break;
	case '1':
		if (unoPress) unoPress = false;
		else unoPress = true;
		cout << "1: " << unoPress << endl;
		break;
	case '2':
		if (dosPress) dosPress = false;
		else dosPress = true;
		cout << "2: " << dosPress << endl;
		break;
	case '3':
		if (tresPress) tresPress = false;
		else tresPress = true;
		cout << "3: " << tresPress << endl;
		break;
	case '4':
		if (cuatroPress) cuatroPress = false;
		else cuatroPress = true;
		cout << "4: " << cuatroPress << endl;
		break;
	case '5':
		if (cincoPress) cincoPress = false;
		else cincoPress = true;
		cout << "5: " << cincoPress << endl;
		break;
	case '6':
		if (seisPress) seisPress = false;
		else seisPress = true;
		cout << "6: " << seisPress << endl;
		break;
	case '7':
		if (sietePress) sietePress = false;
		else sietePress = true;
		cout << "7: " << sietePress << endl;
		break;
	case '8':
		if (ochoPress) ochoPress = false;
		else ochoPress = true;
		cout << "8: " << ochoPress << endl;
		break;
	case 'a':
		if (aPress) aPress = false;
		else aPress = true;
		break;
	case 'r':
		if (rPress) rPress = false;
		else rPress = true;
		break;
	case 'v':
		if (vPress) vPress = false;
		else vPress = true;
		break;
	case 'b':
		if (bPress) bPress = false;
		else bPress = true;
		break;
	case 'f':
		if (zbuffer) zbuffer = false;
		else zbuffer = true;
		break;
	case 'c':
		if (cPress) cPress = false;
		else cPress = true;
		break;
	default:
		cout << "otra tecla" << endl;
		break;
	}
}

void keyInputUp(unsigned char key, int x, int y) {
	prevCursorX = cursorX;
	prevCursorY = cursorY;
	cursorX = x;
	cursorY = y;

	switch (key) {
	case 'x':
		xPress = false;
		break;
	case 'y':
		yPress = false;
		break;
	case 'z':
		zPress = false;
		break;
	default:
		cout << "otra tecla realsed" << endl;
		break;
	}
}

void cursorPos(int x, int y) {
	if (TwEventMouseMotionGLUT(int(x), int(y)))
		return;

	prevCursorX = cursorX;
	prevCursorY = cursorY;
	cursorX = x;
	cursorY = y;

	if (leftPress) {
		int distX = abs(prevCursorX - cursorX);
		int distY = abs(prevCursorY - cursorY);

		if (prevCursorX < cursorX) angleX += distX;
		if (prevCursorX > cursorX) angleX -= distX;
		if (prevCursorY < cursorY) angleY += distY;
		if (prevCursorY > cursorY) angleY -= distY;
	}

	if (rightPress) {
		float distY = abs(prevCursorY - cursorY);

		distY = distY / 10;

		if (prevCursorY < cursorY) translateZ += distY;
		if (prevCursorY > cursorY) translateZ -= distY;
	}

}

void cursorPosPassive(int x, int y) {
	if (TwEventMouseMotionGLUT(int(x), int(y)))
		return;

	prevCursorX = cursorX;
	prevCursorY = cursorY;
	cursorX = x;
	cursorY = y;
	/*
	float distY = abs(prevCursorY - cursorY);

	distY = distY / 10;

	if (xPress) {
		if (prevCursorY < cursorY) scaleX += distY;
		if (prevCursorY > cursorY) scaleX -= distY;
	}

	if (yPress) {
		if (prevCursorY < cursorY) scaleY += distY;
		if (prevCursorY > cursorY) scaleY -= distY;
	}

	if (zPress) {
		if (prevCursorY < cursorY) scaleZ += distY;
		if (prevCursorY > cursorY) scaleZ -= distY;
	}
	*/
}

void updateUserInterface()
{
	mAmbientColor = userInterface->getMaterialAmbientColor();
	mDiffuseColor = userInterface->getMaterialDiffuseColor();
	mSpecularColor = userInterface->getMaterialSpecularColor();
	lAmbientColor = userInterface->getLightAmbientColor();
	lDiffuseColor = userInterface->getLightDiffuseColor();
	lSpecularColor = userInterface->getLightSpecularColor();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	GLfloat one_light[] = { -1.0, 0.0, 0.0, 0.0 },
		two_light[] = { 1.0, 0.0, 0.0, 0.0 },
		three_light[] = { 0.0, 1.0, 0.0, 0.0 },
		four_light[] = { 0.0, -1.0, 0.0, 0.0 };

	if (currentTexEnv != OFF) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		GLint mode;
		
		if (currentTexEnv == SPHERE_MAP) mode = GL_SPHERE_MAP;
		if (currentTexEnv == EYE_LINEAR) mode = GL_EYE_LINEAR;
		if (currentTexEnv == OBJECT_LINEAR) mode = GL_OBJECT_LINEAR;

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
	} else {
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glPushMatrix();

	if (zbuffer) glDepthMask(GL_TRUE);
	else glDepthMask(GL_FALSE);

	if (cPress) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	if (currentCulling == CULLING_BACK) glCullFace(GL_BACK);
	else glCullFace(GL_FRONT);

	if (windowHeight == 0) windowHeight = 1;
	float aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;

	glLoadIdentity();
	gluPerspective(60.0, aspect, nearClipping, farClipping);

	glTranslatef(translateX, translateY, translateZ);
	glRotatef(angleX, 0, 1, 0);
	glRotatef(angleY, 1, 0, 0);
	glScalef(scaleX, scaleY, scaleZ);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mAmbientColor);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mDiffuseColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mSpecularColor);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	for (int i = 0; i < m; i++) {

		if (rPress) {
			glBegin(GL_TRIANGLES);
			glNormal3f(normales[i].x, normales[i].y, normales[i].z);
				glTexCoord2f(0.0, 1.0);
				glVertex3f(vertex[triangles[i].x].x, vertex[triangles[i].x].y, vertex[triangles[i].x].z);
				glTexCoord2f(1.0, 1.0);
				glVertex3f(vertex[triangles[i].y].x, vertex[triangles[i].y].y, vertex[triangles[i].y].z);
				glTexCoord2f(0.5, 0.0);
				glVertex3f(vertex[triangles[i].z].x, vertex[triangles[i].z].y, vertex[triangles[i].z].z);
			glEnd();
		}
		if (aPress) {
			glLineWidth(2.0);
			glColor3f(0.0, 0.0, 0.0);
			glBegin(GL_LINE_LOOP);
				glVertex3f(vertex[triangles[i].x].x, vertex[triangles[i].x].y, vertex[triangles[i].x].z);
				glVertex3f(vertex[triangles[i].y].x, vertex[triangles[i].y].y, vertex[triangles[i].y].z);
				glVertex3f(vertex[triangles[i].z].x, vertex[triangles[i].z].y, vertex[triangles[i].z].z);
			glEnd();
		}
		if (vPress) {
			glPointSize(10);
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_POINTS);
				glVertex3f(vertex[triangles[i].x].x, vertex[triangles[i].x].y, vertex[triangles[i].x].z + 0.000);
				glVertex3f(vertex[triangles[i].y].x, vertex[triangles[i].y].y, vertex[triangles[i].y].z + 0.000);
				glVertex3f(vertex[triangles[i].z].x, vertex[triangles[i].z].y, vertex[triangles[i].z].z + 0.000);
			glEnd();
		}
		if (bPress) {
			glLineWidth(2.0);
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINE_LOOP);
				glVertex3f(centroide[i].x, centroide[i].y, centroide[i].z);
				glVertex3f(centroide[i].x - normales[i].x, centroide[i].y - normales[i].y, centroide[i].z - normales[i].z);
			glEnd();
		}
	}

	glPopMatrix();
	glPushMatrix();

	glLightfv(GL_LIGHT0, GL_POSITION, one_light);
	glLightfv(GL_LIGHT1, GL_POSITION, two_light);
	glLightfv(GL_LIGHT2, GL_POSITION, three_light);
	glLightfv(GL_LIGHT3, GL_POSITION, four_light);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lAmbientColor);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lAmbientColor);
	glLightfv(GL_LIGHT2, GL_AMBIENT, lAmbientColor);
	glLightfv(GL_LIGHT3, GL_AMBIENT, lAmbientColor);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, lDiffuseColor);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lDiffuseColor);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lDiffuseColor);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, lDiffuseColor);

	glLightfv(GL_LIGHT0, GL_SPECULAR, lSpecularColor);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lSpecularColor);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lSpecularColor);
	glLightfv(GL_LIGHT3, GL_SPECULAR, lSpecularColor);

	if (unoPress) glEnable(GL_LIGHT0);
	else glDisable(GL_LIGHT0);

	if (dosPress) glEnable(GL_LIGHT1);
	else glDisable(GL_LIGHT1);

	if (tresPress) glEnable(GL_LIGHT2);
	else glDisable(GL_LIGHT2);

	if (cuatroPress) glEnable(GL_LIGHT3);
	else glDisable(GL_LIGHT3);

	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, white);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 0.0);

	glPointSize(20);
	glBegin(GL_POINTS);
		if (unoPress) glVertex3fv(two_light);
		if (dosPress) glVertex3fv(one_light);
		if (tresPress) glVertex3fv(four_light);
		if (cuatroPress) glVertex3fv(three_light);
	glEnd();

	glPopMatrix();

	TwDraw();
	updateUserInterface();

	glutSwapBuffers();

}

void reshape(GLsizei width, GLsizei height)
{

	if (height == 0) height = 1;
	float aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluPerspective(60.0, aspect, nearClipping, 150);

	userInterface->reshape();
}

void TW_CALL ButtonFarSet(const void* value, void* clientData)
{
	farClipping = *static_cast<const float*>(value);
}

void TW_CALL ButtonFarGet(void* value, void* clientData)
{
	*static_cast<float*>(value) = farClipping;
}

void TW_CALL ButtonNearSet(const void* value, void* clientData)
{
	nearClipping = *static_cast<const float*>(value);
}

void TW_CALL ButtonNearGet(void* value, void* clientData)
{
	*static_cast<float*>(value) = nearClipping;
}

void TW_CALL ButtonShininessSet(const void* value, void* clientData)
{
	shininess = *static_cast<const float*>(value);
}

void TW_CALL ButtonShininessGet(void* value, void* clientData)
{
	*static_cast<float*>(value) = shininess;
}

void TW_CALL Buttontexture(void* clientDate)
{
	loadTexture();
}

bool initUserInterface()
{
	if (!TwInit(TW_OPENGL, NULL))
		return false;

	userInterface = CUserInterface::Instance();

	TwAddSeparator(userInterface->getTwBar(), "Separator one", NULL);

	TwAddVarCB(userInterface->getTwBar(), "Shininess", TW_TYPE_FLOAT, ButtonShininessSet, ButtonShininessGet, NULL, " Min=0.0 Max=100 Step=0.1 KeyIncr=5 KeyDecr=6 ");
	TwAddVarCB(userInterface->getTwBar(), "Near Clipping", TW_TYPE_FLOAT, ButtonNearSet, ButtonNearGet, NULL, " Min=0.1 Max=100 Step=0.1 KeyIncr=1 KeyDecr=2 ");
	TwAddVarCB(userInterface->getTwBar(), "Far Clipping", TW_TYPE_FLOAT, ButtonFarSet, ButtonFarGet, NULL, " Min=0.1 Max=100 Step=0.1 KeyIncr=3 KeyDecr=4 ");

	TwAddSeparator(userInterface->getTwBar(), "Separator two", NULL);

	TwAddButton(userInterface->getTwBar(), "Load Texture", Buttontexture, NULL, " label='Load Texture' ");

	TwAddSeparator(userInterface->getTwBar(), "Separator three", NULL);

	TwEnumVal TexEnvEV[4] = { {OFF, "OFF"}, {EYE_LINEAR, "GL_EYE_LINEAR"}, {SPHERE_MAP, "GL_SPHERE_MAP"}, {OBJECT_LINEAR, "GL_OBJECT_LINEAR"} };
	TwType textEnvType = TwDefineEnum("TexEnvType", TexEnvEV, 4);
	TwAddVarRW(userInterface->getTwBar(), "Texture Env", textEnvType, &currentTexEnv, " keyIncr='<' keyDecr='>' ");

	TwAddSeparator(userInterface->getTwBar(), "Separator four", NULL);

	TwEnumVal cullingEV[2] = { {CULLING_BACK, "Back"}, {CULLING_FRONT, "Front"} };
	TwType cullingType = TwDefineEnum("CullingType", cullingEV, 2);
	TwAddVarRW(userInterface->getTwBar(), "Culling Face", cullingType, &currentCulling, " keyIncr='<' keyDecr='>' ");

	mAmbientColor = userInterface->getMaterialAmbientColor();
	mDiffuseColor = userInterface->getMaterialDiffuseColor();
	mSpecularColor = userInterface->getMaterialSpecularColor();
	lAmbientColor = userInterface->getLightAmbientColor();
	lDiffuseColor = userInterface->getLightDiffuseColor();
	lSpecularColor = userInterface->getLightSpecularColor();

	return true;
}

void terminate(void)
{
	delete userInterface;
	TwTerminate();
}

int main(int argc, char** argv)
{
	currentCulling = CULLING_BACK;

	windowWidth = 1200;
	windowHeight = 680;
	windowPosX = windowPosY = 0;

	n = m = k = num = 0;
	angleX = angleY = angleZ = 0;
	rotateX = rotateY = rotateZ = 0;
	translateX = translateY = 0;
	translateZ = -5;
	scaleX = scaleY = scaleZ = 0.5;
	leftPress = rightPress = middlePress = false;
	xPress = yPress = zPress = false;
	rPress = true;
	aPress = vPress = bPress = false;
	unoPress = dosPress = tresPress = cuatroPress = true;
	cincoPress = seisPress = sietePress = ochoPress = true;
	zbuffer = true;
	cPress = false;
	nearClipping = 0.1;
	farClipping = 100;
	shininess = 0.0;
	pMin.x = pMin.y = pMin.z = 1000000;
	pMax.x = pMax.y = pMax.z = -1;

	if (load()) {
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
		glutInitWindowSize(windowWidth, windowHeight);
		glutInitWindowPosition(windowPosX, windowPosY);
		glutCreateWindow(title);
		glutCreateMenu(NULL);

		if (!initUserInterface()) return EXIT_FAILURE;

		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		atexit(terminate);
		glutIdleFunc(display);
		glutMouseFunc(mouseButton);
		glutKeyboardFunc(keyInput);
		glutKeyboardUpFunc(keyInputUp);
		glutMotionFunc(cursorPos);
		glutPassiveMotionFunc(cursorPosPassive);
		TwGLUTModifiersFunc(glutGetModifiers);
		initGL();
		glutMainLoop();
	}

	return 0;
}