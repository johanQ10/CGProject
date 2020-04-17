#pragma once

#include "GL/AntTweakBar.h"
#include <iostream>
#include <string>

using std::string;

//Singleton user interface class

class CUserInterface
{
private:
	static CUserInterface* mInterface; //Holds the instance of the class
	TwBar* mUserInterface;
	float mAmbientColor[4];
	float mDiffuseColor[4];
	float mSpecularColor[4];
	float lAmbientColor[4];
	float lDiffuseColor[4];
	float lSpecularColor[4];

public:
	///Method to obtain the only instance of the calls
	static CUserInterface* Instance();
	~CUserInterface();
	void reshape();
	void show();
	void hide();
	void setMaterialAmbientColor(float* color);
	void setMaterialDiffuseColor(float* color);
	void setMaterialSpecularColor(float* color);
	void setLightAmbientColor(float* color);
	void setLightDiffuseColor(float* color);
	void setLightSpecularColor(float* color);
	TwBar* getTwBar();
	float* getMaterialAmbientColor();
	float* getMaterialDiffuseColor();
	float* getMaterialSpecularColor();
	float* getLightAmbientColor();
	float* getLightDiffuseColor();
	float* getLightSpecularColor();

private:
	///Private constructor
	CUserInterface();
};