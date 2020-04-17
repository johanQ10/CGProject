#include "UserInterface.h"

extern int windowWidth, windowHeight;

// Global static pointer used to ensure a single instance of the class.
CUserInterface* CUserInterface::mInterface = NULL;

/**
* Creates an instance of the class
*
* @return the instance of this class
*/
CUserInterface* CUserInterface::Instance()
{
	if (!mInterface)   // Only allow one instance of class to be generated.
		mInterface = new CUserInterface();

	return mInterface;
}

CUserInterface::CUserInterface()
{
	mUserInterface = TwNewBar("Figure 3D");

	TwDefine("Figure refresh = '0.0001f'");
	TwDefine("Figure resizable = false");
	TwDefine("Figure fontresizable = false");
	TwDefine("Figure movable = true");
	TwDefine("Figure visible = true");
	TwDefine("Figure position = '20 20'");
	TwDefine("Figure size = '220 320'");

	TwAddVarRW(mUserInterface, "mAmbientColor", TW_TYPE_COLOR4F, &mAmbientColor, "label = 'Material Ambient Color'");
	TwAddVarRW(mUserInterface, "mDiffuseColor", TW_TYPE_COLOR4F, &mDiffuseColor, "label = 'Material Diffuse Color'");
	TwAddVarRW(mUserInterface, "mSpecularColor", TW_TYPE_COLOR4F, &mSpecularColor, "label = 'Material Specular Color'");

	TwAddSeparator(mUserInterface, "Separator zero", NULL);

	TwAddVarRW(mUserInterface, "lAmbientColor", TW_TYPE_COLOR4F, &lAmbientColor, "label = 'Light Ambient Color'");
	TwAddVarRW(mUserInterface, "lDiffuseColor", TW_TYPE_COLOR4F, &lDiffuseColor, "label = 'Light Diffuse Color'");
	TwAddVarRW(mUserInterface, "lSpecularColor", TW_TYPE_COLOR4F, &lSpecularColor, "label = 'Light Specular Color'");

	mAmbientColor[0] = 0.33;
	mAmbientColor[1] = 0.22;
	mAmbientColor[2] = 0.03;
	mAmbientColor[3] = 1.0;

	mDiffuseColor[0] = 0.78;
	mDiffuseColor[1] = 0.57;
	mDiffuseColor[2] = 0.11;
	mDiffuseColor[3] = 1.0;

	mSpecularColor[0] = 0.99;
	mSpecularColor[3] = 0.91;
	mSpecularColor[1] = 0.91;
	mSpecularColor[2] = 1.0;

	lAmbientColor[0] = lAmbientColor[1] = lAmbientColor[2] = 0;
	lAmbientColor[3] = 1;
	lDiffuseColor[0] = lDiffuseColor[1] = lDiffuseColor[2] = lDiffuseColor[3] = 1;
	lSpecularColor[0] = lSpecularColor[1] = lSpecularColor[2] = lSpecularColor[3] = 1;

}

CUserInterface::~CUserInterface()
{
}

void CUserInterface::reshape()
{
	TwWindowSize(windowWidth, windowHeight);
}

void CUserInterface::show()
{
	TwDefine("Figure visible = true");
}

void CUserInterface::hide()
{
	TwDefine("Figure visible = false");
}

void CUserInterface::setMaterialAmbientColor(float* color)
{
	mAmbientColor[0] = color[0];
	mAmbientColor[1] = color[1];
	mAmbientColor[2] = color[2];
	mAmbientColor[3] = color[3];
}

void CUserInterface::setMaterialDiffuseColor(float* color)
{
	mDiffuseColor[0] = color[0];
	mDiffuseColor[1] = color[1];
	mDiffuseColor[2] = color[2];
	mDiffuseColor[3] = color[3];
}

void CUserInterface::setMaterialSpecularColor(float* color)
{
	mSpecularColor[0] = color[0];
	mSpecularColor[1] = color[1];
	mSpecularColor[2] = color[2];
	mSpecularColor[3] = color[3];
}

void CUserInterface::setLightAmbientColor(float* color)
{
	lAmbientColor[0] = color[0];
	lAmbientColor[1] = color[1];
	lAmbientColor[2] = color[2];
	lAmbientColor[3] = color[3];
}

void CUserInterface::setLightDiffuseColor(float* color)
{
	lDiffuseColor[0] = color[0];
	lDiffuseColor[1] = color[1];
	lDiffuseColor[2] = color[2];
	lDiffuseColor[3] = color[3];
}

void CUserInterface::setLightSpecularColor(float* color)
{
	lSpecularColor[0] = color[0];
	lSpecularColor[1] = color[1];
	lSpecularColor[2] = color[2];
	lSpecularColor[3] = color[3];
}

TwBar* CUserInterface::getTwBar() {
	return mUserInterface;
}

float* CUserInterface::getMaterialAmbientColor()
{
	return mAmbientColor;
}

float* CUserInterface::getMaterialDiffuseColor()
{
	return mDiffuseColor;
}

float* CUserInterface::getMaterialSpecularColor()
{
	return mSpecularColor;
}

float* CUserInterface::getLightAmbientColor()
{
	return lAmbientColor;
}

float* CUserInterface::getLightDiffuseColor()
{
	return lDiffuseColor;
}

float* CUserInterface::getLightSpecularColor()
{
	return lSpecularColor;
}