#define STB_IMAGE_IMPLEMENTATION
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glfw3.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <Math.h>
#include <fstream>
#include <string>
#include <vector>
#include "Main.h"
#include "Point.h"
#include "UserInterface.h"
#include "GL/std_image.h"
#include "GL/glm/vec3.hpp" // glm::vec3
#include "GL/glm/vec4.hpp" // glm::vec4
#include "GL/glm/mat4x4.hpp" // glm::mat4
#include "GL/glm/ext/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale
#include "GL/glm/ext/matrix_clip_space.hpp" // glm::perspective
#include "GL/glm/gtc/constants.hpp" // glm::pi
#include "GL/glm/gtc/type_ptr.hpp"
//#include "GL/glad.h"

using namespace std;
using namespace glm;
using std::vector;

typedef enum { CULLING_BACK, CULLING_FRONT } Culling;
typedef enum { OFF, EYE_LINEAR, SPHERE_MAP, OBJECT_LINEAR } TexEnv;

GLFWwindow* window;
Culling currentCulling;
TexEnv currentTexEnv;
char title[] = "Shader - Johan Quintero";
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
bool isTexture;

bool load()
{
	char name[100];

	cout << "Ingrese el nombre del archivo: ";
	cin >> name;

	ifstream inFile;
	string off = "";

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

	stbi_set_flip_vertically_on_load(true);
	data_tex = stbi_load(name, &w_image, &h_image, &nrChannels, 0);

	cout << "w_image: " << w_image << endl;
	cout << "h_image: " << h_image << endl;
	cout << "nrChannels: " << nrChannels << endl;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (data_tex) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w_image, h_image, 0, GL_RGB, GL_UNSIGNED_BYTE, data_tex);
		glGenerateMipmap(GL_TEXTURE_2D);

		isTexture = true;
	}
	else {
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data_tex);
}

void initGL()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glShadeModel(GL_SMOOTH);

	for (int i = 0; i < m; i++) {

		pMin.x = MIN(pMin.x, vertex[(int) triangles[i].x].x);
		pMin.x = MIN(pMin.x, vertex[(int) triangles[i].y].x);
		pMin.x = MIN(pMin.x, vertex[(int) triangles[i].z].x);

		pMin.y = MIN(pMin.y, vertex[(int) triangles[i].x].y);
		pMin.y = MIN(pMin.y, vertex[(int) triangles[i].y].y);
		pMin.y = MIN(pMin.y, vertex[(int) triangles[i].z].y);

		pMin.z = MIN(pMin.z, vertex[(int) triangles[i].x].z);
		pMin.z = MIN(pMin.z, vertex[(int) triangles[i].y].z);
		pMin.z = MIN(pMin.z, vertex[(int) triangles[i].z].z);

		pMax.x = MAX(pMax.x, vertex[(int) triangles[i].x].x);
		pMax.x = MAX(pMax.x, vertex[(int) triangles[i].y].x);
		pMax.x = MAX(pMax.x, vertex[(int) triangles[i].z].x);

		pMax.y = MAX(pMax.y, vertex[(int) triangles[i].x].y);
		pMax.y = MAX(pMax.y, vertex[(int) triangles[i].y].y);
		pMax.y = MAX(pMax.y, vertex[(int) triangles[i].z].y);

		pMax.z = MAX(pMax.z, vertex[(int) triangles[i].x].z);
		pMax.z = MAX(pMax.z, vertex[(int) triangles[i].y].z);
		pMax.z = MAX(pMax.z, vertex[(int) triangles[i].z].z);

		CPoint P, Q, normal;

		P.x = vertex[(int) triangles[i].z].x - vertex[(int) triangles[i].x].x;
		P.y = vertex[(int) triangles[i].z].y - vertex[(int) triangles[i].x].y;
		P.z = vertex[(int) triangles[i].z].z - vertex[(int) triangles[i].x].z;

		Q.x = vertex[(int) triangles[i].y].x - vertex[(int) triangles[i].x].x;
		Q.y = vertex[(int) triangles[i].y].y - vertex[(int) triangles[i].x].y;
		Q.z = vertex[(int) triangles[i].y].z - vertex[(int) triangles[i].x].z;

		normal.x = P.y * Q.z - P.z * Q.y;
		normal.y = P.z * Q.x - P.x * Q.z;
		normal.z = P.x * Q.y - P.y * Q.x;

		GLfloat modulo = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		modulo = (GLfloat) 1.0 / modulo;

		normal.x *= modulo;
		normal.y *= modulo;
		normal.z *= modulo;

		CPoint centro = CPoint((vertex[(int) triangles[i].x].x + vertex[(int) triangles[i].y].x + vertex[(int) triangles[i].z].x) / 3,
			(vertex[(int) triangles[i].x].y + vertex[(int) triangles[i].y].y + vertex[(int) triangles[i].z].y) / 3,
			(vertex[(int) triangles[i].x].z + vertex[(int) triangles[i].y].z + vertex[(int) triangles[i].z].z) / 3);

		normales.push_back(normal);
		centroide.push_back(centro);
	}

}

void mouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (TwEventMouseButtonGLFW(button, action))
		return;

	double x, y;
	glfwGetCursorPos(window, &x, &y);

	prevCursorX = cursorX;
	prevCursorY = cursorY;
	cursorX = (int) x;
	cursorY = (int) y;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !leftPress && !rightPress)
	{
		leftPress = true;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && !leftPress && !rightPress)
	{
		rightPress = true;
	}

	if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_RELEASE) {
		leftPress = false;
		rightPress = false;

		GLFWcursor* cursorArrow = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		glfwSetCursor(window, cursorArrow);
	}
}

void keyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (TwEventKeyGLFW(key, action))
		return;

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_X:
			if (xPress) xPress = true;
			else xPress = false;
			break;
		case GLFW_KEY_Y:
			if (yPress) yPress = true;
			else yPress = false;
			break;
		case GLFW_KEY_Z:
			if (zPress) zPress = true;
			else zPress = false;
			break;
		case GLFW_KEY_1:
			if (unoPress) unoPress = false;
			else unoPress = true;
			break;
		case GLFW_KEY_2:
			if (dosPress) dosPress = false;
			else dosPress = true;
			break;
		case GLFW_KEY_3:
			if (tresPress) tresPress = false;
			else tresPress = true;
			break;
		case GLFW_KEY_4:
			if (cuatroPress) cuatroPress = false;
			else cuatroPress = true;
			break;
		case GLFW_KEY_A:
			if (aPress) aPress = false;
			else aPress = true;
			break;
		case GLFW_KEY_R:
			if (rPress) rPress = false;
			else rPress = true;
			break;
		case GLFW_KEY_V:
			if (vPress) vPress = false;
			else vPress = true;
			break;
		case GLFW_KEY_B:
			if (bPress) bPress = false;
			else bPress = true;
			break;
		case GLFW_KEY_F:
			if (zbuffer) zbuffer = false;
			else zbuffer = true;
			break;
		case GLFW_KEY_C:
			if (cPress) cPress = false;
			else cPress = true;
			break;
		default:
			cout << "otra tecla" << endl;
			break;
		}
	}
}

void cursorPos(GLFWwindow* window, double x, double y)
{
	if (TwEventMousePosGLFW(int(x), int(y)))
		return;

	prevCursorX = cursorX;
	prevCursorY = cursorY;
	cursorX = (int) x;
	cursorY = (int) y;

	if (leftPress) {
		float distX = (float) abs(prevCursorX - cursorX);
		float distY = (float) abs(prevCursorY - cursorY);

		distX = distX / 100;
		distY = distY / 100;

		if (prevCursorX < cursorX) angleX += distX;
		if (prevCursorX > cursorX) angleX -= distX;
		if (prevCursorY < cursorY) angleY += distY;
		if (prevCursorY > cursorY) angleY -= distY;
	}

	if (rightPress) {
		float distX = (float) abs(prevCursorX - cursorX);
		float distY = (float) abs(prevCursorY - cursorY);

		distX = distX / 100;
		distY = distY / 10;

		if (prevCursorX < cursorX) translateX += distX;
		if (prevCursorX > cursorX) translateX -= distX;
		if (prevCursorY < cursorY) translateZ += distY;
		if (prevCursorY > cursorY) translateZ -= distY;
	}
}

void charInput(GLFWwindow* window, unsigned int scanChar)
{
	if (TwEventCharGLFW(scanChar, GLFW_PRESS))
		return;
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

const char* vertexShaderLamp = "#version 430\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 projection;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * transform * vec4(aPos, 1.0);\n"
"}\0";

const char* fragmentShaderLamp = "#version 430\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0);\n"
"}\n\0";

const char* vertexShaderInit = "#version 430\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec3 aNormal;\n"
"out vec3 Color;\n"
"out vec3 Normal;\n"
"out vec3 FragPos;\n"
"uniform mat4 model;"
"uniform mat4 transform;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * transform * vec4(aPos, 1.0);\n"
"   FragPos = vec3(model * vec4(aPos, 1.0));"
"   Color = aColor;\n"
"   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"}\0";

const char* fragmentShaderInit = "#version 430\n"
"struct Material {\n"
"	vec3 ambient;\n"
"	vec3 diffuse;\n"
"	vec3 specular;\n"
"};\n"
"struct Light {\n"
"	vec3 position;\n"
"	vec3 ambient;\n"
"	vec3 diffuse;\n"
"	vec3 specular;\n"
"};\n"
"out vec4 FragColor;\n"
"in vec3 Color;\n"
"in vec3 FragPos;\n"
"in vec3 Normal;\n"
"uniform vec3 viewPos;\n"
"uniform float shininess;\n"
"uniform Light light;\n"
"uniform Material material;\n"
"void main()\n"
"{\n"
"   // ambient\n"
"   float ambientStrength = 0.3;\n"
"   vec3 AmbientColor = ambientStrength * light.ambient * material.ambient;\n"
"   // diffuse\n"
"   vec3 norm = Normal;\n"
"   vec3 lightDir = normalize(light.position - FragPos);\n"
"   float DiffuseFactor = max(dot(norm, lightDir), 0.0);\n"
"   vec3 DiffuseColor = vec3(0, 0, 0);\n"
"   vec3 SpecularColor = vec3(0, 0, 0);\n"
"   if (DiffuseFactor > 0) {\n"
"       DiffuseColor = light.diffuse * DiffuseFactor * material.diffuse;\n"
"       // specular\n"
"       vec3 VertexToEye = normalize(viewPos - FragPos);\n"
"       vec3 LightReflect = normalize(reflect(-lightDir, norm));\n"
"       float SpecularFactor = dot(VertexToEye, LightReflect);\n"
"       if (SpecularFactor > 0) {\n"
"          SpecularFactor = pow(SpecularFactor, shininess);\n"
"          SpecularColor = light.specular * SpecularFactor * material.specular;\n"
"       }\n"
"   }\n"
"   FragColor = vec4((AmbientColor + DiffuseColor + SpecularColor), 1.0);\n"
"}\n\0";

const char* vertexShaderTex = "#version 430\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec3 aNormal;\n"
"layout (location = 3) in vec2 aTexCoord;\n"
"out vec3 Color;\n"
"out vec3 Normal;\n"
"out vec3 FragPos;\n"
"out vec2 TexCoord;\n"
"uniform mat4 model;"
"uniform mat4 transform;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * transform * vec4(aPos, 1.0);\n"
"   FragPos = vec3(model * vec4(aPos, 1.0));"
"   Color = aColor;\n"
"   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"   TexCoord = aTexCoord;\n"
"}\0";

const char* fragmentShaderTex = "#version 430\n"
"struct Material {\n"
"	vec3 ambient;\n"
"	vec3 diffuse;\n"
"	vec3 specular;\n"
"};\n"
"struct Light {\n"
"	vec3 position;\n"
"	vec3 ambient;\n"
"	vec3 diffuse;\n"
"	vec3 specular;\n"
"};\n"
"out vec4 FragColor;\n"
"in vec3 Color;\n"
"in vec3 FragPos;\n"
"in vec3 Normal;\n"
"in vec2 TexCoord;\n"
"uniform vec3 viewPos;\n"
"uniform float shininess;\n"
"uniform Light light;\n"
"uniform Material material;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   // ambient\n"
"   float ambientStrength = 0.3;\n"
"   vec3 AmbientColor = ambientStrength * light.ambient * material.ambient;\n"
"   // diffuse\n"
"   vec3 norm = Normal;\n"
"   vec3 lightDir = normalize(light.position - FragPos);\n"
"   float DiffuseFactor = max(dot(norm, lightDir), 0.0);\n"
"   vec3 DiffuseColor = vec3(0, 0, 0);\n"
"   vec3 SpecularColor = vec3(0, 0, 0);\n"
"   if (DiffuseFactor > 0) {\n"
"       DiffuseColor = light.diffuse * DiffuseFactor * material.diffuse;\n"
"       // specular\n"
"       vec3 VertexToEye = normalize(viewPos - FragPos);\n"
"       vec3 LightReflect = normalize(reflect(-lightDir, norm));\n"
"       float SpecularFactor = dot(VertexToEye, LightReflect);\n"
"       if (SpecularFactor > 0) {\n"
"          SpecularFactor = pow(SpecularFactor, shininess);\n"
"          SpecularColor = light.specular * SpecularFactor * material.specular;\n"
"       }\n"
"   }\n"
"   FragColor = texture(ourTexture, TexCoord) * vec4((AmbientColor + DiffuseColor + SpecularColor), 1.0);\n"
"}\n\0";

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	vec3 lightPos(-5.0, 0.0, -5.0);
	vec3 viewPos(0.0, 0.0, -10.0);

	// vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	
	if (isTexture) glShaderSource(vertexShader, 1, &vertexShaderTex, NULL);
	else glShaderSource(vertexShader, 1, &vertexShaderInit, NULL);
	
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}
	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	if (isTexture) glShaderSource(fragmentShader, 1, &fragmentShaderTex, NULL);
	else glShaderSource(fragmentShader, 1, &fragmentShaderInit, NULL);
	
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}
	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	if (zbuffer) glDepthMask(GL_TRUE);
	else glDepthMask(GL_FALSE);

	if (cPress) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	if (currentCulling == CULLING_BACK) glCullFace(GL_BACK);
	else glCullFace(GL_FRONT);

	if (windowHeight == 0) windowHeight = 1;
	float aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;

	glLoadIdentity();

	mat4 projection = perspective((GLfloat) radians(45.0), aspect, nearClipping, farClipping);
	
	mat4 transform = translate(mat4(1.0f), vec3(translateX, translateY, translateZ));
	transform = rotate(transform, angleX, vec3(0.0f, 1.0f, 0.0f));
	transform = rotate(transform, angleY, vec3(1.0f, 0.0f, 0.0f));

	float ra, ga, ba, rd, gd, bd, rs, gs, bs;

	ra = mAmbientColor[0];
	ga = mAmbientColor[1];
	ba = mAmbientColor[2];

	rd = mDiffuseColor[0];
	gd = mDiffuseColor[1];
	bd = mDiffuseColor[2];

	rs = mSpecularColor[0];
	gs = mSpecularColor[1];
	bs = mSpecularColor[2];

	for (int i = 0; i < m; i++) {
		float x1 = (float) vertex[(int) triangles[i].x].x, y1 = (float) vertex[(int) triangles[i].x].y, z1 = (float) vertex[(int) triangles[i].x].z;
		float x2 = (float) vertex[(int) triangles[i].y].x, y2 = (float) vertex[(int) triangles[i].y].y, z2 = (float) vertex[(int) triangles[i].y].z;
		float x3 = (float) vertex[(int) triangles[i].z].x, y3 = (float) vertex[(int) triangles[i].z].y, z3 = (float) vertex[(int) triangles[i].z].z;

		float xc = centroide[i].x, yc = centroide[i].y, zc = centroide[i].z;
		float xn = centroide[i].x - normales[i].x, yn = centroide[i].y - normales[i].y, zn = centroide[i].z - normales[i].z;
		float nx = normales[i].x, ny = normales[i].y, nz = normales[i].z;

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);

		float vertices[] = {
			// positions  // colors     // normals    // texture coords
			x1, y1, z1,   ra, ga, ba,   nx, ny, nz,   0.0f, 0.0f,
			x2, y2, z2,   ra, ga, ba,   nx, ny, nz,   1.0f, 0.0f,
			x3, y3, z3,   ra, ga, ba,   nx, ny, nz,   0.5f, 1.0f,
			// line
			x1, y1, z1,   rd, gd, bd,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
			x2, y2, z2,   rd, gd, bd,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
			x3, y3, z3,   rd, gd, bd,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
			// vertex
			x1, y1, z1,   rs, gs, bs,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
			x2, y2, z2,   rs, gs, bs,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
			x3, y3, z3,   rs, gs, bs,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
			// normal
			xc, yc, zc,   rs, gs, bs,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
			xn, yn, zn,   rs, gs, bs,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		};

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// normal attribute
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		// texture attribute
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));

		if (isTexture) 
			glEnableVertexAttribArray(3);
		else glDisableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glUseProgram(shaderProgram);

		GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

		GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(transform));

		mat4 model = mat4(1.0f);
		GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

		glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), viewPos[0], viewPos[1], viewPos[2]);
		glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);

		glUniform3f(glGetUniformLocation(shaderProgram, "light.position"), lightPos[0], lightPos[1], lightPos[2]);
		glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), lAmbientColor[0], lAmbientColor[1], lAmbientColor[2]);
		glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), lDiffuseColor[0], lDiffuseColor[1], lDiffuseColor[2]);
		glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), lSpecularColor[0], lSpecularColor[1], lSpecularColor[2]);

		glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), mAmbientColor[0], mAmbientColor[1], mAmbientColor[2]);
		glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), mDiffuseColor[0], mDiffuseColor[1], mDiffuseColor[2]);
		glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), mSpecularColor[0], mSpecularColor[1], mSpecularColor[2]);

		glBindVertexArray(VAO);

		if (rPress) glDrawArrays(GL_TRIANGLES, 0, 3);
		if (aPress) {
			glLineWidth(2.0);
			glDrawArrays(GL_LINE_LOOP, 3, 3);
		}
		if (vPress) {
			glPointSize(10);
			glDrawArrays(GL_POINTS, 6, 3);
		}
		if (bPress) {
			glLineWidth(2.0);
			glDrawArrays(GL_LINE_LOOP, 9, 2);
		}
	
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// -- LAMP --
	int vsLamp = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsLamp, 1, &vertexShaderLamp, NULL);
	glCompileShader(vsLamp);
	int fsLamp = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsLamp, 1, &fragmentShaderLamp, NULL);
	glCompileShader(fsLamp);
	
	int shaderLamp = glCreateProgram();
	glAttachShader(shaderLamp, vsLamp);
	glAttachShader(shaderLamp, fsLamp);
	glLinkProgram(shaderLamp);

	glDeleteShader(vsLamp);
	glDeleteShader(fsLamp);

	unsigned int VBOLamp, VAOLamp;
	glGenVertexArrays(1, &VAOLamp);
	glGenBuffers(1, &VBOLamp);

	float inc = 1;

	float vLamp[] = {
		// positions
		-(lightPos[0] - inc), -(lightPos[1] + inc), -lightPos[2],
		-(lightPos[0] - inc), -(lightPos[1] - inc), -lightPos[2],
		-(lightPos[0] + inc), -(lightPos[1] - inc), -lightPos[2],
		-(lightPos[0] + inc), -(lightPos[1] + inc), -lightPos[2],
	};

	glBindVertexArray(VAOLamp);
	glBindBuffer(GL_ARRAY_BUFFER, VBOLamp);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vLamp), vLamp, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(shaderLamp);

	glUniformMatrix4fv(glGetUniformLocation(shaderLamp, "projection"), 1, GL_FALSE, value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderLamp, "transform"), 1, GL_FALSE, value_ptr(transform));

	glBindVertexArray(VAOLamp);

	glDrawArrays(GL_QUADS, 0, 4);

	glDeleteVertexArrays(1, &VAOLamp);
	glDeleteBuffers(1, &VBOLamp);
}

void reshape(GLFWwindow* window, int width, int height)
{

	if (height == 0) height = 1;
	float aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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

void destroy()
{
	delete userInterface;
	TwTerminate();
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool initGlfw()
{
	if (!glfwInit())
		return false;

	window = glfwCreateWindow(windowWidth, windowHeight, title, NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, (vidMode->width - windowWidth) >> 1, (vidMode->height - windowHeight) >> 1);

	glfwSetWindowSizeCallback(window, reshape);
	glfwSetKeyCallback(window, keyInput);
	glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetCursorPosCallback(window, cursorPos);
	glfwSetCharCallback(window, charInput);

	return true;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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
	nearClipping = (float) 0.1;
	farClipping = 100;
	shininess = 0.0;
	pMin.x = pMin.y = pMin.z = 1000000;
	pMax.x = pMax.y = pMax.z = -1;
	isTexture = false;

	if (!initGlfw() || !initUserInterface())
		return EXIT_FAILURE;
	
	GLenum err = glewInit();

	if (GLEW_OK != err) {
		cout << "Error GLEW: " << glewGetErrorString(err) << endl;
	}

	reshape(window, windowWidth, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (load()) {
		initGL();
		while (!glfwWindowShouldClose(window)) {
			processInput(window);
			display();
			TwDraw();
			updateUserInterface();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	destroy();

	return EXIT_SUCCESS;

	return 0;
}