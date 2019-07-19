#include "UserInterface.h"

// Global static pointer used to ensure a single instance of the class.
CUserInterface * CUserInterface::mInterface = NULL;

/**
* Creates an instance of the class
*
* @return the instance of this class
*/
CUserInterface * CUserInterface::Instance() 
{
	if (!mInterface)   // Only allow one instance of class to be generated.
		 mInterface = new CUserInterface();
 
   return mInterface;
}

CUserInterface::CUserInterface()
{
	windowWidth = 800;
	windowHeight = 600;
	mUserInterface = TwNewBar("Tarea 1");

	TwDefine("Model refresh = '0.0001f'");
	TwDefine("Model resizable = false");
	TwDefine("Model fontresizable = false");
	TwDefine("Model movable = false");
	TwDefine("Model position = '20 20'");
	TwDefine("Model size = '220 320'");

	TwEnumVal modelLigtsEV[] = { {blinn, "Blinn-Phong"}, {oren, "Oren-Nayar"}, {cook, "Cook-Torrance"}};
	TwType mlType;

	mlType = TwDefineEnum("", modelLigtsEV, 3);

	//Interface data
    mModelTranslation[0] = 0.5f; 
    mModelTranslation[1] = 4.0f;
    mModelTranslation[2] = 6.0f;
	showLigthsDir = true;
	showLigthsPoint = true;
	showLigthsSpecular = true;
	lightP[0] = 10.0f;
	lightP[1] = 14.0f;
	lightP[2] = -10.0f;

	g_MatAmbient[0] = 0.0f;g_MatDiffuse[0] = 1.0f;g_MatSpecular[0] = 1.0f;
	g_MatAmbient[1] = 0.0f;g_MatDiffuse[1] = 1.0f;g_MatSpecular[1] = 1.0f;
	g_MatAmbient[2] = 0.0f;g_MatDiffuse[2] = 1.0f;g_MatSpecular[2] = 1.0f;

	g_LigAmbient[0] = 1.0f;g_LigDiffuse[0] = 1.0f;g_LigSpecular[0] = 1.0f;
	g_LigAmbient[1] = 1.0f;g_LigDiffuse[1] = 1.0f;g_LigSpecular[1] = 1.0f;
	g_LigAmbient[2] = 1.0f;g_LigDiffuse[2] = 1.0f;g_LigSpecular[2] = 1.0f;

	g_LightDir = true;
	g_LightPoint = false;
	g_LightSpot = false;
	lightSelected = 1;
	heightScale = 0.1f;
	reflectance = 1.0f;
	refractance = 1.0f;

	g_Texture = true;
	g_Parall = true;
	g_Reflect = false;
	g_Refract = false;
	g_Shininess = 32;
	g_Roughness = 15;
	g_cutOff = 12.5f;
	g_OuterCutOff = 17.5f;
	g_Constant = 1.0f;
	g_Linear = 0.09f;
	g_Quadratic = 0.032f;
	
	TwAddVarRW(mUserInterface, "Model Material", mlType, &mLights, " group='Model' ");
	TwAddVarRW(mUserInterface, "Point Light", TW_TYPE_INT32, &lightSelected, " group='Model' step=1 min=1 max=2");
	TwAddVarRW(mUserInterface, "Texture", TW_TYPE_BOOLCPP, &g_Texture, " group='Textures' ");
	TwAddVarRW(mUserInterface, "Parallax", TW_TYPE_BOOLCPP, &g_Parall, " group='Textures' ");
	TwAddVarRW(mUserInterface, "Reflectance", TW_TYPE_FLOAT, &reflectance, " group='Textures' step=0.1");
	TwAddVarRW(mUserInterface, "Refractance", TW_TYPE_FLOAT, &refractance, " group='Textures' step=0.1");
	TwAddVarRW(mUserInterface, "HeightScale", TW_TYPE_FLOAT, &heightScale, " group='Textures' step=0.1");
    TwAddVarRW(mUserInterface, "Reflection", TW_TYPE_BOOLCPP, &g_Reflect, " group='Model' ");
    TwAddVarRW(mUserInterface, "Refraction", TW_TYPE_BOOLCPP, &g_Refract, " group='Model' ");
	TwAddVarRW(mUserInterface, "Translation X", TW_TYPE_FLOAT, &mModelTranslation[0], " group='Model' step=0.1 ");
	TwAddVarRW(mUserInterface, "Translation Y", TW_TYPE_FLOAT, &mModelTranslation[1], " group='Model' step=0.1 ");
	TwAddVarRW(mUserInterface, "Translation Z", TW_TYPE_FLOAT, &mModelTranslation[2], " group='Model' step=0.1 ");
	TwAddSeparator(mUserInterface, "", NULL);
	TwAddVarRW(mUserInterface, "ON/OFF - Direct", TW_TYPE_BOOLCPP, &showLigthsDir, " group='Lights'");
	TwAddVarRW(mUserInterface, "ON/OFF - Point", TW_TYPE_BOOLCPP, &showLigthsPoint, " group='Lights'");
	TwAddVarRW(mUserInterface, "ON/OFF - Spec", TW_TYPE_BOOLCPP, &showLigthsSpecular, " group='Lights'");
    TwAddVarRW(mUserInterface, "LightDir", TW_TYPE_DIR3F, &lightP, "group='Lights' ");
	TwAddSeparator(mUserInterface, "", NULL);
    TwAddVarRW(mUserInterface, "Directional", TW_TYPE_BOOLCPP, &g_LightDir, " group='Light Type'");
    TwAddVarRW(mUserInterface, "Point", TW_TYPE_BOOLCPP, &g_LightPoint, " group='Light Type'");
    TwAddVarRW(mUserInterface, "Spot", TW_TYPE_BOOLCPP, &g_LightSpot, " group='Light Type'");
	TwAddSeparator(mUserInterface, "", NULL);
    TwAddVarRW(mUserInterface, "Ambient", TW_TYPE_COLOR3F, &g_MatAmbient, " group='Materials' ");
    TwAddVarRW(mUserInterface, "Diffuse", TW_TYPE_COLOR3F, &g_MatDiffuse, " group='Materials' ");
    TwAddVarRW(mUserInterface, "Specular", TW_TYPE_COLOR3F, &g_MatSpecular, " group='Materials' ");
	TwAddSeparator(mUserInterface, "", NULL);
    TwAddVarRW(mUserInterface, "lightAmbient", TW_TYPE_COLOR3F, &g_LigAmbient, " group='Light Properties' ");
    TwAddVarRW(mUserInterface, "lightDiffuse", TW_TYPE_COLOR3F, &g_LigDiffuse, " group='Light Properties' ");
    TwAddVarRW(mUserInterface, "lightSpecular", TW_TYPE_COLOR3F, &g_LigSpecular, " group='Light Properties' ");
    TwAddVarRW(mUserInterface, "Shininess", TW_TYPE_FLOAT, &g_Shininess, " group='Light Properties' min=0 step=0.1");
    TwAddVarRW(mUserInterface, "Roughness", TW_TYPE_FLOAT, &g_Roughness, " group='Light Properties' min=0 step=0.01");
	TwAddVarRW(mUserInterface, "cutOff", TW_TYPE_FLOAT, &g_cutOff, " group='Light Properties' min=0 step=0.1");
	TwAddVarRW(mUserInterface, "OuterCutOff", TW_TYPE_FLOAT, &g_OuterCutOff, " group='Light Properties' min=0 step=0.1");
	TwAddVarRW(mUserInterface, "Constant", TW_TYPE_FLOAT, &g_Constant, " group='Light Properties' min=0 step=0.01");
	TwAddVarRW(mUserInterface, "Linear", TW_TYPE_FLOAT, &g_Linear, " group='Light Properties' min=0 step=0.01");
	TwAddVarRW(mUserInterface, "Quadratic", TW_TYPE_FLOAT, &g_Quadratic, " group='Light Properties' min=0 step=0.01");
	TwAddSeparator(mUserInterface, "", NULL);
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