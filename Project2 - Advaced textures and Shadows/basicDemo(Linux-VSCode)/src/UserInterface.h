#pragma once

#include <AntTweakBar.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <string>

using std::string;

typedef enum {
	blinn,
	oren,
	cook
} ModelLights;

//Singleton user interface class

class CUserInterface {
public:
	//Interface data
	static CUserInterface * mInterface; //Holds the instance of the class
	TwBar *mUserInterface;
	glm::vec3 mModelTranslation;	
	bool showLigthsDir;
	bool showLigthsPoint;
	bool showLigthsSpecular;
	float lightP[3];
	float heightScale;
	float reflectance;
	float refractAmb;
	float refractObj;
	bool g_Texture;
	bool g_Reflect;
	bool g_Refract;
	bool g_Parall;
	float windowWidth, windowHeight;

	//Materials
	float g_MatAmbient[3];
	float g_MatDiffuse[3];
	float g_MatSpecular[3];
	
	//Lights
	ModelLights mLights = blinn;

	//Light Types
	bool g_LightDir;
	bool g_LightPoint;
	bool g_LightSpot;
	int lightSelected;

	//Light Properties
	float g_LigAmbient[3];
	float g_LigDiffuse[3];
	float g_LigSpecular[3];
	float g_Shininess;
	float g_Roughness;
	float g_cutOff;
	float g_OuterCutOff;
	float g_Constant;
	float g_Linear;
	float g_Quadratic;

public:
	///Method to obtain the only instance of the calls
	static CUserInterface * Instance();
	~CUserInterface();
	void reshape();
	void show();
	void hide();

private:
	///Private constructor
	CUserInterface(); 
};