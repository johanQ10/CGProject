#define GLFW_INCLUDE_GLU
#include "GL/glew.h"
#include "GL/glfw3.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void updateUserInterface();

void display();

void reshape(GLFWwindow* window, int width, int height);

void keyInput(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouseButton(GLFWwindow* window, int button, int action, int mods);

void cursorPos(GLFWwindow* window, double x, double y);

void charInput(GLFWwindow* window, unsigned int scanChar);

void destroy();

bool initGlfw();

bool initUserInterface();