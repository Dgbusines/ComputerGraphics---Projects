#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <stb_image.h>

#include "UserInterface.h"
#include "Shader.h"
#include "objLoader.h"
#include "Navigation.h"
#include <vector>
#include <string>
using namespace std;

// Window current width
unsigned int windowWidth = 800;
// Window current height
unsigned int windowHeight = 600;

// Window title
const char *windowTitle = "Tarea_1";
// Window pointer
GLFWwindow *window;

// Shader object
Shader *blinnShader, *lampShader, *sunShader, *orenShader, *cookShader, *skyboxShader, *depthShader, *blendShader, *debugDepthQuad;
// Index (GPU) of the geometry buffer
unsigned int VBO[4];
unsigned int skyboxVBO;
unsigned int VBO1[2];
//unsigned int VBO1;
// Index (GPU) vertex array object
unsigned int VAO[4];
unsigned int lightVAO[2];
unsigned int skyboxVAO;
//unsigned int VAO1;
// Index (GPU) of the texture
unsigned int textureID[8];
unsigned int skybox;
vector<std::string> faces{
    "assets/textures/right.jpg",
    "assets/textures/left.jpg",
    "assets/textures/top.jpg",
    "assets/textures/bottom.jpg",
    "assets/textures/front.jpg",
    "assets/textures/back.jpg"
};

//New data
glm::mat4 MVP, model, view, projection;
COBJ *model01 = new COBJ();
COBJ *model02 = new COBJ();
COBJ *model03 = new COBJ();
COBJ *model04 = new COBJ();
COBJ *model05 = new COBJ();
COBJ *floorScene = new COBJ();
COBJ *skyboxOBJ = new COBJ();
navigate *mov = new navigate();
CUserInterface *userInterface;
void mouseCallback(GLFWwindow *window, int action, int button, int mods);
void scrollCallback(GLFWwindow *window, double x, double y);
void cursorPos(GLFWwindow *window, double x, double y);
void keyInput(GLFWwindow *window, int key, int scancode, int action, int mods);
void renderDepht();
void configDepht();
void debugDepht();
unsigned int depthMapFBO;
unsigned int depthMap;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
float near_plane = 0.01f, far_plane = 7.5f;
glm::mat4 lightSpaceMatrix;


bool camera = true;
bool firstTimem2 = true;
bool firstTimem3 = false;

void initInterface()
{
    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(windowWidth, windowHeight);

    userInterface = CUserInterface::Instance();
}
/**
 * Handles the window resize
 * @param{GLFWwindow} window pointer
 * @param{int} new width of the window
 * @param{int} new height of the window
 * */
void resize(GLFWwindow *window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    // Sets the OpenGL viewport size and position
    glViewport(0, 0, windowWidth, windowHeight);
    // Resize interface
    userInterface->reshape();
}
/**
 * Initialize the glfw library
 * @returns{bool} true if everything goes ok
 * */
bool initWindow()
{
    // Initialize glfw
    glfwInit();
    // Sets the Opegl context to Opengl 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Creates the window
    window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);

    // The window couldn't be created
    if (!window)
    {
        std::cout << "Failed to create the glfw window" << std::endl;
        glfwTerminate(); // Stops the glfw program
        return false;
    }

    // Creates the glfwContext, this has to be made before calling initGlad()
    glfwMakeContextCurrent(window);

    // Window resize callback
    glfwSetFramebufferSizeCallback(window, resize);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetCursorPosCallback(window, cursorPos);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyInput);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return true;
}
/**
 * Initialize the glad library
 * @returns{bool} true if everything goes ok
 * */
bool initGlad()
{
    // Initialize glad
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    // If something went wrong during the glad initialization
    if (!status)
    {
        std::cout << status << std::endl;
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}
/**
 * Initialize the opengl context
 * */
void initGL()
{
    // Enables the z-buffer test
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Sets the ViewPort
    glViewport(0, 0, windowWidth, windowHeight);
    // Sets the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
/**
 * Builds all the geometry buffers and
 * loads them up into the GPU
 * (Builds a simple triangle)
 * */
void buildGeometry()
{
    model01->load("assets/obj/quad.obj");
    model02->load("assets/obj/quad.obj");
    model03->load("assets/obj/quad.obj");
    model04->load("assets/obj/poke.obj");
	model05->load("assets/obj/poke.obj");
	floorScene->load("assets/obj/floor.obj");
    skyboxOBJ->load("assets/obj/skybox.obj");

    model04->traslation[0] = 0.5f;
    model04->traslation[1] = 4.0f;
    model04->traslation[2] = 6.0f;

    model05->traslation[0] = -0.5f;
    model05->traslation[1] = 4.0f;
    model05->traslation[2] = 6.0f;
    //skybox
    float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    // Creates on GPU the vertex array
    for (int i = 0; i < 4; i++)
    {
        glGenVertexArrays(1, &VAO[i]);
        // Creates on GPU the vertex buffer object
        glGenBuffers(1, &VBO[i]);
        // Binds the vertex array to set all the its properties
        glBindVertexArray(VAO[i]);
        // Sets the buffer geometry data
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        if (i == 0)
            glBufferData(GL_ARRAY_BUFFER, model01->sizeModel * sizeof(float), &model01->objModel[0], GL_STATIC_DRAW);
        else if (i == 1)
            glBufferData(GL_ARRAY_BUFFER, model02->sizeModel * sizeof(float), &model02->objModel[0], GL_STATIC_DRAW);
        else if (i == 2)
            glBufferData(GL_ARRAY_BUFFER, model03->sizeModel * sizeof(float), &model03->objModel[0], GL_STATIC_DRAW);
		else 
			glBufferData(GL_ARRAY_BUFFER, floorScene->sizeModel * sizeof(float), &floorScene->objModel[0], GL_STATIC_DRAW);

        // Sets the vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(3 * sizeof(float)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(5 * sizeof(float)));
        
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(8 * sizeof(float)));
        
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(11 * sizeof(float)));
        glBindVertexArray(0);
    }

    //light
    for (int i = 0; i < 2; i++)
    {
        glGenVertexArrays(1, &lightVAO[i]);
        glGenBuffers(1, &VBO1[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO1[i]);
        glBindVertexArray(lightVAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO1[i]);
        if (i == 0)
            glBufferData(GL_ARRAY_BUFFER, model04->sizeModel * sizeof(float), &model04->objModel[0], GL_STATIC_DRAW);
        else
            glBufferData(GL_ARRAY_BUFFER, model05->sizeModel * sizeof(float), &model05->objModel[0], GL_STATIC_DRAW);
        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)0);
        glBindVertexArray(0);
    }

    //Skybox
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, skyboxOBJ->sizeModel * sizeof(float), &skyboxOBJ->objModel[0], GL_STATIC_DRAW);
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)0);
    glBindVertexArray(0);


    projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    view = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    model = glm::mat4(1.0f);
    MVP = projection * view * model; // Remember, matrix multiplication is the other way around

	configDepht();
}
/**
 * Loads a texture into the GPU
 * @param{const char} path of the texture file
 * @returns{unsigned int} GPU texture index
 * */
unsigned int loadTexture(const char *path)
{
    unsigned int id;
    // Creates the texture on GPU
    glGenTextures(1, &id);
    // Loads the texture
    int textureWidth, textureHeight, numberOfChannels;
    // Flips the texture when loads it because in opengl the texture coordinates are flipped
	stbi_set_flip_vertically_on_load(false);
    // Loads the texture file data
    unsigned char *data = stbi_load(path, &textureWidth, &textureHeight, &numberOfChannels, 0);
    if (data)
    {
        // Gets the texture channel format
        GLenum format;
        switch (numberOfChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        }

        // Binds the texture
        glBindTexture(GL_TEXTURE_2D, id);
        // Creates the texture
        glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
        // Creates the texture mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set the filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "ERROR:: Unable to load texture " << path << std::endl;
        glDeleteTextures(1, &id);
    }
    // We dont need the data texture anymore because is loaded on the GPU
    stbi_image_free(data);

    return id;
}
/**
 * Loads a texture into the GPU
 * @param{vector<std::string>} path of the skybox file
 * @returns{unsigned int} GPU skybox index
 * */
unsigned int loadCubemap()
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
/**
 * Initialize everything
 * @returns{bool} true if everything goes ok
 * */
bool init()
{
    // Initialize the window, and the glad components
    if (!initWindow() | !initGlad())
        return false;

    // Initialize the opengl context
    initGL();

    // Init interface
    initInterface();

    // Loads the shader
    blinnShader = new Shader("assets/shaders/blinnPhong.vert", "assets/shaders/blinnPhong.frag");
    orenShader = new Shader("assets/shaders/blinnPhong.vert", "assets/shaders/oren.frag");
    lampShader = new Shader("assets/shaders/lamp.vert", "assets/shaders/lamp.frag");
    sunShader = new Shader("assets/shaders/lamp.vert", "assets/shaders/sun.frag");
	skyboxShader = new Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
	depthShader = new Shader("assets/shaders/shadowDeph.vert", "assets/shaders/shadowDeph.frag");
	blendShader = new Shader("assets/shaders/blend.vert", "assets/shaders/blend.frag");
	debugDepthQuad = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    // Loads all the geometry into the GPU
    buildGeometry();
    // Loads the texture into the GPU
    textureID[0] = loadTexture("assets/textures/brickwall.jpg");
    textureID[1] = loadTexture("assets/textures/brickwall_normal.jpg");
    textureID[2] = loadTexture("assets/textures/bricks2_disp.jpg");
	textureID[3] = loadTexture("assets/textures/bricks2_normal.jpg");
	textureID[4] = loadTexture("assets/textures/wood.png");
	textureID[5] = loadTexture("assets/textures/toy_box_normal.png");
	textureID[6] = loadTexture("assets/textures/toy_box_disp.png");
	textureID[7] = loadTexture("assets/textures/window.png");
    skybox = loadCubemap();

    return true;
}
/**
 * Process the keyboard input
 * There are ways of implementing this function through callbacks provide by
 * the GLFW API, check the GLFW documentation to find more
 * @param{GLFWwindow} window pointer
 * */
void processKeyboardInput(GLFWwindow *window)
{
    // Checks if the escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // Tells glfw to close the window as soon as possible
        glfwSetWindowShouldClose(window, true);

    // Checks if the r key is pressed
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        // Reloads the shader
        delete blinnShader;
        delete orenShader;
        delete skyboxShader;
		delete depthShader;
		delete blendShader;
        delete lampShader;
        delete sunShader;
        delete debugDepthQuad;
        blinnShader = new Shader("assets/shaders/blinnPhong.vert", "assets/shaders/blinnPhong.frag");
        orenShader = new Shader("assets/shaders/blinnPhong.vert", "assets/shaders/oren.frag");
        lampShader = new Shader("assets/shaders/lamp.vert", "assets/shaders/lamp.frag");
        sunShader = new Shader("assets/shaders/lamp.vert", "assets/shaders/sun.frag");
        skyboxShader = new Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
		depthShader = new Shader("assets/shaders/shadowDeph.vert", "assets/shaders/shadowDeph.frag");
	    debugDepthQuad = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		blendShader = new Shader("assets/shaders/blend.vert", "assets/shaders/blend.frag");
    }
}
/**
 * Render Function
 * */
void render(Shader *shader)
{
    // Clears the color and depth buffers from the frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glEnable(GL_CULL_FACE);

    //Keyboard monitor
    if (camera)
        mov->movements(window);

    glm::vec3 lightPos;
    lightPos.x = userInterface->lightP[0];
    lightPos.y = userInterface->lightP[1];
    lightPos.z = userInterface->lightP[2];
    projection = mov->getProjectionMatrix();
    view = mov->getViewMatrix();
    model = glm::mat4(1.0f);

    glm::mat4 model_02 = glm::mat4(1.0f);
    glm::mat4 model_03 = glm::mat4(1.0f);
    glm::mat4 model1 = glm::mat4(1.0f);
    model1 = glm::translate(model1, lightPos);
    model1 = glm::scale(model1, glm::vec3(0.2f));
    model_02 = glm::translate(model_02, glm::vec3(-0.5f, 3.0f, 5.0f));
    model_03 = glm::translate(model_03, glm::vec3(-1.5f, 3.0f, -7.0f));

    MVP = projection * view * model; // Remember, matrix multiplication is the other way around

    for (int i = 0; i < 4; i++)
    {
        if (userInterface->mLights == blinn)
        {
            //Model Properties
            shader->use();
            shader->setInt("diffuseMap", 2);
			shader->setInt("normalMap", 3);
			shader->setInt("depthMap", 4);
            if (i == 0)
            {
                if (userInterface->g_Texture)
                {
					glActiveTexture(GL_TEXTURE2); // activate the texture unit first before binding texture
					glBindTexture(GL_TEXTURE_2D, textureID[0]);
					glActiveTexture(GL_TEXTURE3); // activate the texture unit first before binding texture
					glBindTexture(GL_TEXTURE_2D, textureID[1]);
					glActiveTexture(GL_TEXTURE5); // activate the texture unit first before binding texture
					glBindTexture(GL_TEXTURE_2D, depthMap);
                }
                shader->setMat4("model", model);
            }
            else if (i == 1)
            {
                if (userInterface->g_Texture)
                {
                    glActiveTexture(GL_TEXTURE2); // activate the texture unit first before binding texture
                    glBindTexture(GL_TEXTURE_2D, textureID[2]);
                    glActiveTexture(GL_TEXTURE3); // activate the texture unit first before binding texture
                    glBindTexture(GL_TEXTURE_2D, textureID[3]);
					glActiveTexture(GL_TEXTURE5); // activate the texture unit first before binding texture
					glBindTexture(GL_TEXTURE_2D, depthMap);
                }
                shader->setMat4("model", model_02);
            }
            else if (i == 2)
            {
                if (userInterface->g_Texture)
                {
                    glActiveTexture(GL_TEXTURE2); // activate the texture unit first before binding texture
                    glBindTexture(GL_TEXTURE_2D, textureID[4]);
                    glActiveTexture(GL_TEXTURE3); // activate the texture unit first before binding texture
                    glBindTexture(GL_TEXTURE_2D, textureID[5]);
					glActiveTexture(GL_TEXTURE5); // activate the texture unit first before binding texture
					glBindTexture(GL_TEXTURE_2D, depthMap);
					if (userInterface->g_Parall) {
						glActiveTexture(GL_TEXTURE4); // activate the texture unit first before binding texture
						glBindTexture(GL_TEXTURE_2D, textureID[6]);
					}
					shader->setBool("getIn", 1);
                }
                shader->setMat4("model", model_03);
            }
			else
			{
				if (userInterface->g_Texture)
				{
					glActiveTexture(GL_TEXTURE2); // activate the texture unit first before binding texture
					glBindTexture(GL_TEXTURE_2D, textureID[0]);
					glActiveTexture(GL_TEXTURE3); // activate the texture unit first before binding texture
					glBindTexture(GL_TEXTURE_2D, textureID[1]);
					glActiveTexture(GL_TEXTURE5); // activate the texture unit first before binding texture
					glBindTexture(GL_TEXTURE_2D, depthMap);
				}
				shader->setMat4("model", model);
			}
            shader->setMat4("view", view);
            shader->setMat4("projection", projection);

            //Light Properties
            shader->setVec3("material.ambient", userInterface->g_MatAmbient[0], userInterface->g_MatAmbient[1], userInterface->g_MatAmbient[2]);
            shader->setVec3("material.diffuse", userInterface->g_MatDiffuse[0], userInterface->g_MatDiffuse[1], userInterface->g_MatDiffuse[2]);
            shader->setVec3("material.specular", userInterface->g_MatSpecular[0], userInterface->g_MatSpecular[1], userInterface->g_MatSpecular[2]);
            shader->setFloat("material.shininess", userInterface->g_Shininess);
            shader->setFloat("material.roughness", userInterface->g_Roughness);


            //Directional
            shader->setVec3("lightDirect.direction", lightPos);

            //Point
            shader->setVec3("lightPoint.position1", model04->traslation[0], model04->traslation[1], model04->traslation[2]);
            shader->setVec3("lightPoint.position2", model05->traslation[0], model05->traslation[1], model05->traslation[2]);
            shader->setFloat("lightPoint.constant", userInterface->g_Constant);
            shader->setFloat("lightPoint.linear", userInterface->g_Linear);
            shader->setFloat("lightPoint.quadratic", userInterface->g_Quadratic);

            //Spot
            shader->setVec3("lightSpot.position", mov->getCameraPos());
            shader->setVec3("lightSpot.direction", mov->getCameraFront());
            shader->setFloat("lightSpot.cutOff", glm::cos(glm::radians(userInterface->g_cutOff)));
            shader->setFloat("lightSpot.outerCutOff", glm::cos(glm::radians(userInterface->g_OuterCutOff)));
            shader->setFloat("lightSpot.constant", userInterface->g_Constant);
            shader->setFloat("lightSpot.linear", userInterface->g_Linear);
            shader->setFloat("lightSpot.quadratic", userInterface->g_Quadratic);

            //Color
            if (userInterface->g_LightDir)
            {
                shader->setVec3("lightDirect.ambient", userInterface->g_LigAmbient[0], userInterface->g_LigAmbient[1], userInterface->g_LigAmbient[2]);
                shader->setVec3("lightDirect.diffuse", userInterface->g_LigDiffuse[0], userInterface->g_LigDiffuse[1], userInterface->g_LigDiffuse[2]);
                shader->setVec3("lightDirect.specular", userInterface->g_LigSpecular[0], userInterface->g_LigSpecular[1], userInterface->g_LigSpecular[2]);
            }
            if (userInterface->g_LightPoint)
            {
                shader->setVec3("lightPoint.ambient1", model04->matAmb[0], model04->matAmb[1], model04->matAmb[2]);
                shader->setVec3("lightPoint.diffuse1", model04->matDiff[0], model04->matDiff[1], model04->matDiff[2]);
                shader->setVec3("lightPoint.specular1", model04->matSpec[0], model04->matSpec[1], model04->matSpec[2]);
                shader->setVec3("lightPoint.ambient2", model05->matAmb[0], model05->matAmb[1], model05->matAmb[2]);
                shader->setVec3("lightPoint.diffuse2", model05->matDiff[0], model05->matDiff[1], model05->matDiff[2]);
                shader->setVec3("lightPoint.specular2", model05->matSpec[0], model05->matSpec[1], model05->matSpec[2]);
            }
            if (userInterface->g_LightSpot)
            {
                shader->setVec3("lightSpot.ambient", userInterface->g_LigAmbient[0], userInterface->g_LigAmbient[1], userInterface->g_LigAmbient[2]);
                shader->setVec3("lightSpot.diffuse", userInterface->g_LigDiffuse[0], userInterface->g_LigDiffuse[1], userInterface->g_LigDiffuse[2]);
                shader->setVec3("lightSpot.specular", userInterface->g_LigSpecular[0], userInterface->g_LigSpecular[1], userInterface->g_LigSpecular[2]);
            }
            shader->setBool("onOffDir", userInterface->showLigthsDir);
            shader->setBool("onOffPoint", userInterface->showLigthsPoint);
            shader->setBool("onOffSpec", userInterface->showLigthsSpecular);
			shader->setBool("texLoad", userInterface->g_Texture);
			shader->setBool("parall", userInterface->g_Parall);
            shader->setBool("reflection", userInterface->g_Reflect);
            shader->setBool("refraction", userInterface->g_Refract);
            shader->setInt("skybox", 1);
			shader->setVec3("viewPos", mov->getCameraPos());
			shader->setFloat("heightScale", userInterface->heightScale);
			shader->setFloat("reflectance", userInterface->reflectance);
			shader->setFloat("refractance", userInterface->refractance);
			shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

            glBindVertexArray(VAO[i]);
            if (userInterface->g_Reflect || userInterface->g_Refract)
            {
                glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
                glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
            }
            if (i == 0)
                glDrawArrays(GL_TRIANGLES, 0, model01->sizeModel / 14);
            else if (i == 1)
                glDrawArrays(GL_TRIANGLES, 0, model02->sizeModel / 14);
            else if (i == 2)
				glDrawArrays(GL_TRIANGLES, 0, model03->sizeModel / 14);
			else 
				glDrawArrays(GL_TRIANGLES, 0, floorScene->sizeModel / 14);
            glBindVertexArray(0);
			shader->setBool("getIn", 0);
        }
        else if (userInterface->mLights == oren)
        {
            //Model Properties
            orenShader->use();
            if (i == 0)
            {
                if (userInterface->g_Texture)
                {
                    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
                    glBindTexture(GL_TEXTURE_2D, textureID[i]);
                }
                orenShader->setMat4("model", model);
            }
            else if (i == 1)
            {
                if (userInterface->g_Texture)
                {
                    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
                    glBindTexture(GL_TEXTURE_2D, textureID[i]);
                }
                orenShader->setMat4("model", model_02);
            }
            else
            {
                if (userInterface->g_Texture)
                {
                    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
                    glBindTexture(GL_TEXTURE_2D, textureID[i]);
                }
                orenShader->setMat4("model", model_03);
            }
            orenShader->setMat4("view", view);
            orenShader->setMat4("projection", projection);

            //Light Properties
            orenShader->setVec3("material.ambient", userInterface->g_MatAmbient[0], userInterface->g_MatAmbient[1], userInterface->g_MatAmbient[2]);
            orenShader->setVec3("material.diffuse", userInterface->g_MatDiffuse[0], userInterface->g_MatDiffuse[1], userInterface->g_MatDiffuse[2]);
            orenShader->setVec3("material.specular", userInterface->g_MatSpecular[0], userInterface->g_MatSpecular[1], userInterface->g_MatSpecular[2]);
            orenShader->setFloat("material.shininess", userInterface->g_Shininess);
            orenShader->setFloat("material.roughness", userInterface->g_Roughness);
            orenShader->setVec3("viewPos", mov->getCameraPos());

            //Directional
            orenShader->setVec3("lightDirect.position", lightPos);

            //Point
            orenShader->setVec3("lightPoint.position1", model04->traslation[0], model04->traslation[1], model04->traslation[2]);
            orenShader->setVec3("lightPoint.position2", model05->traslation[0], model05->traslation[1], model05->traslation[2]);
            orenShader->setFloat("lightPoint.constant", userInterface->g_Constant);
            orenShader->setFloat("lightPoint.linear", userInterface->g_Linear);
            orenShader->setFloat("lightPoint.quadratic", userInterface->g_Quadratic);

            //Spot
            orenShader->setVec3("lightSpot.position", mov->getCameraPos());
            orenShader->setVec3("lightSpot.direction", mov->getCameraFront());
            orenShader->setFloat("lightSpot.cutOff", glm::cos(glm::radians(userInterface->g_cutOff)));
            orenShader->setFloat("lightSpot.outerCutOff", glm::cos(glm::radians(userInterface->g_OuterCutOff)));
            orenShader->setFloat("lightSpot.constant", userInterface->g_Constant);
            orenShader->setFloat("lightSpot.linear", userInterface->g_Linear);
            orenShader->setFloat("lightSpot.quadratic", userInterface->g_Quadratic);

            //Color
            if (userInterface->g_LightDir)
            {
                orenShader->setVec3("lightDirect.ambient", userInterface->g_LigAmbient[0], userInterface->g_LigAmbient[1], userInterface->g_LigAmbient[2]);
                orenShader->setVec3("lightDirect.diffuse", userInterface->g_LigDiffuse[0], userInterface->g_LigDiffuse[1], userInterface->g_LigDiffuse[2]);
                orenShader->setVec3("lightDirect.specular", userInterface->g_LigSpecular[0], userInterface->g_LigSpecular[1], userInterface->g_LigSpecular[2]);
            }
            if (userInterface->g_LightPoint)
            {
                orenShader->setVec3("lightPoint.ambient1", model04->matAmb[0], model04->matAmb[1], model04->matAmb[2]);
                orenShader->setVec3("lightPoint.diffuse1", model04->matDiff[0], model04->matDiff[1], model04->matDiff[2]);
                orenShader->setVec3("lightPoint.specular1", model04->matSpec[0], model04->matSpec[1], model04->matSpec[2]);
                orenShader->setVec3("lightPoint.ambient2", model05->matAmb[0], model05->matAmb[1], model05->matAmb[2]);
                orenShader->setVec3("lightPoint.diffuse2", model05->matDiff[0], model05->matDiff[1], model05->matDiff[2]);
                orenShader->setVec3("lightPoint.specular2", model05->matSpec[0], model05->matSpec[1], model05->matSpec[2]);
            }
            if (userInterface->g_LightSpot)
            {
                orenShader->setVec3("lightSpot.ambient", userInterface->g_LigAmbient[0], userInterface->g_LigAmbient[1], userInterface->g_LigAmbient[2]);
                orenShader->setVec3("lightSpot.diffuse", userInterface->g_LigDiffuse[0], userInterface->g_LigDiffuse[1], userInterface->g_LigDiffuse[2]);
                orenShader->setVec3("lightSpot.specular", userInterface->g_LigSpecular[0], userInterface->g_LigSpecular[1], userInterface->g_LigSpecular[2]);
            }
            orenShader->setBool("onOffDir", userInterface->showLigthsDir);
            orenShader->setBool("onOffPoint", userInterface->showLigthsPoint);
            orenShader->setBool("onOffSpec", userInterface->showLigthsSpecular);
            orenShader->setBool("texLoad", userInterface->g_Texture);

            glBindVertexArray(VAO[i]);
            if (i == 0)
                glDrawArrays(GL_TRIANGLES, 0, model01->sizeModel / 8);
            else if (i == 1)
                glDrawArrays(GL_TRIANGLES, 0, model02->sizeModel / 8);
            else
                glDrawArrays(GL_TRIANGLES, 0, model03->sizeModel / 8);
            glBindVertexArray(0);
        }
    }

    if (userInterface->lightSelected == 1)
    {
        if (firstTimem2)
        {
            userInterface->mModelTranslation.x = model04->traslation[0];
            userInterface->mModelTranslation.y = model04->traslation[1];
            userInterface->mModelTranslation.z = model04->traslation[2];
            userInterface->g_LigAmbient[0] = model04->matAmb[0];
            userInterface->g_LigAmbient[1] = model04->matAmb[1];
            userInterface->g_LigAmbient[2] = model04->matAmb[2];

            userInterface->g_LigDiffuse[0] = model04->matDiff[0];
            userInterface->g_LigDiffuse[1] = model04->matDiff[1];
            userInterface->g_LigDiffuse[2] = model04->matDiff[2];

            userInterface->g_LigSpecular[0] = model04->matSpec[0];
            userInterface->g_LigSpecular[1] = model04->matSpec[1];
            userInterface->g_LigSpecular[2] = model04->matSpec[2];
            firstTimem2 = false;
            firstTimem3 = true;
        }
        model_02 = glm::translate(model_02, userInterface->mModelTranslation);
        model_02 = glm::scale(model_02, glm::vec3(0.2f));
        model_03 = glm::translate(model_03, glm::vec3(model05->traslation[0], model05->traslation[1], model05->traslation[2]));
        model_03 = glm::scale(model_03, glm::vec3(0.2f));
        model04->traslation[0] = userInterface->mModelTranslation[0];
        model04->traslation[1] = userInterface->mModelTranslation[1];
        model04->traslation[2] = userInterface->mModelTranslation[2];

        model04->matAmb[0] = userInterface->g_LigAmbient[0];
        model04->matAmb[1] = userInterface->g_LigAmbient[1];
        model04->matAmb[2] = userInterface->g_LigAmbient[2];

        model04->matDiff[0] = userInterface->g_LigDiffuse[0];
        model04->matDiff[1] = userInterface->g_LigDiffuse[1];
        model04->matDiff[2] = userInterface->g_LigDiffuse[2];

        model04->matSpec[0] = userInterface->g_LigSpecular[0];
        model04->matSpec[1] = userInterface->g_LigSpecular[1];
        model04->matSpec[2] = userInterface->g_LigSpecular[2];
    }
    else
    {
        if (firstTimem3)
        {
            userInterface->mModelTranslation.x = model05->traslation[0];
            userInterface->mModelTranslation.y = model05->traslation[1];
            userInterface->mModelTranslation.z = model05->traslation[2];
            userInterface->g_LigAmbient[0] = model05->matAmb[0];
            userInterface->g_LigAmbient[1] = model05->matAmb[1];
            userInterface->g_LigAmbient[2] = model05->matAmb[2];

            userInterface->g_LigDiffuse[0] = model05->matDiff[0];
            userInterface->g_LigDiffuse[1] = model05->matDiff[1];
            userInterface->g_LigDiffuse[2] = model05->matDiff[2];

            userInterface->g_LigSpecular[0] = model05->matSpec[0];
            userInterface->g_LigSpecular[1] = model05->matSpec[1];
            userInterface->g_LigSpecular[2] = model05->matSpec[2];
            firstTimem2 = true;
            firstTimem3 = false;
        }
        model_02 = glm::translate(model_02, glm::vec3(model04->traslation[0], model04->traslation[1], model04->traslation[2]));
        model_02 = glm::scale(model_02, glm::vec3(0.2f));
        model_03 = glm::translate(model_03, userInterface->mModelTranslation);
        model_03 = glm::scale(model_03, glm::vec3(0.2f));
        model05->traslation[0] = userInterface->mModelTranslation[0];
        model05->traslation[1] = userInterface->mModelTranslation[1];
        model05->traslation[2] = userInterface->mModelTranslation[2];

        model05->matAmb[0] = userInterface->g_LigAmbient[0];
        model05->matAmb[1] = userInterface->g_LigAmbient[1];
        model05->matAmb[2] = userInterface->g_LigAmbient[2];

        model05->matDiff[0] = userInterface->g_LigDiffuse[0];
        model05->matDiff[1] = userInterface->g_LigDiffuse[1];
        model05->matDiff[2] = userInterface->g_LigDiffuse[2];

        model05->matSpec[0] = userInterface->g_LigSpecular[0];
        model05->matSpec[1] = userInterface->g_LigSpecular[1];
        model05->matSpec[2] = userInterface->g_LigSpecular[2];
    }

    //Lights
    //Lamps
    for (int i = 0; i < 2; i++)
    {
        lampShader->use();
        if (i == 0)
            lampShader->setMat4("model", model_02);
        else
            lampShader->setMat4("model", model_03);
        lampShader->setMat4("view", view);
        lampShader->setMat4("projection", projection);
        if (userInterface->g_LightPoint)
        {
            if (i == 0)
                lampShader->setVec3("colLamp", model04->matDiff[0], model04->matDiff[1], model04->matDiff[2]);
            else
                lampShader->setVec3("colLamp", model05->matDiff[0], model05->matDiff[1], model05->matDiff[2]);
        }
        lampShader->setBool("onOffPoint", userInterface->showLigthsPoint);

        glBindVertexArray(lightVAO[i]);
        glDrawArrays(GL_TRIANGLES, 0, model04->sizeModel / 14);
        glDrawArrays(GL_TRIANGLES, 0, model05->sizeModel / 14);
        glBindVertexArray(0);
    }

    //Sun
    sunShader->use();
    sunShader->setMat4("model", model1);
    sunShader->setMat4("view", view);
    sunShader->setMat4("projection", projection);
    sunShader->setBool("onOffDir", userInterface->showLigthsDir);

    glBindVertexArray(lightVAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, model04->sizeModel / 14);
    glBindVertexArray(0);

    //Skybox
    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();
    skyboxShader->setInt("skybox", 0);
    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
		glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

	//Transparent box
	model = glm::translate(model, glm::vec3(-6.0f, 0.0f, 0.0f));
	blendShader->use();
	blendShader->setInt("transparent", 0);
	blendShader->setMat4("model", model);
	blendShader->setMat4("view", view);
	blendShader->setMat4("projection", projection);
	sunShader->setBool("onOffDir", userInterface->showLigthsDir);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID[7]);

	glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, model01->sizeModel / 14);
	glBindVertexArray(0);


}
/**
 * App main loop
 * */
void update()
{
    // Loop until something tells the window, that it has to be closed
    while (!glfwWindowShouldClose(window))
    {
        // Checks for keyboard inputs
        processKeyboardInput(window);

		//Renders deph of scene to texture
		renderDepht();

        // Renders everything
        render(blinnShader);

        //Render Debug
        //debugDepht();
		
        if (!camera)
			TwDraw();

		// Swap the buffer
		glfwSwapBuffers(window);

        // Check and call events
        glfwPollEvents();

    }
}
/**
 * App starting point
 * @param{int} number of arguments
 * @param{char const *[]} running arguments
 * @returns{int} app exit code
 * */
int main(int argc, char const *argv[])
{
    // Initialize all the app components
    if (!init())
    {
        // Something went wrong
        std::cin.ignore();
        return -1;
    }

    std::cout << "=====================================================" << std::endl
              << "        Press Escape to close the program            " << std::endl
              << "=====================================================" << std::endl;

    // Starts the app main loop
    update();

    // Deletes the texture from the gpu
    for (int i = 0; i < 4; i++)
    {
        glDeleteTextures(1, &textureID[i]);
        // Deletes the vertex array from the GPU
        glDeleteVertexArrays(1, &VAO[i]);
        // Deletes the vertex object from the GPU
        glDeleteBuffers(1, &VBO[i]);
    }
    // Destroy the shader
    delete blinnShader;
    delete orenShader;
    delete skyboxShader;
	delete depthShader;
	delete blendShader;
    delete lampShader;
    delete sunShader;
    delete debugDepthQuad;

    //Stops the Tw window
    TwTerminate();
    // Stops the glfw program
    glfwTerminate();

    return 0;
}

void mouseCallback(GLFWwindow *window, int action, int button, int mods)
{

    TwMouseButtonID button1 = (TwMouseButtonID)button;
    TwMouseAction action1 = (TwMouseAction)action;
    if (TwMouseButton(action1, button1))
        return;
}
void scrollCallback(GLFWwindow *window, double x, double y)
{
    if (camera)
        mov->scroll(x, y);
}
void cursorPos(GLFWwindow *window, double x, double y)
{
    if (TwMouseMotion(int(x), int(y)))
        return;

    if (camera)
        mov->mouse(window, x, y);
}
void keyInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (TwKeyPressed(key, action))
        return;

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        //Camera movement ON/OFF
        camera = !camera;
        if (!camera)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
        {
            glfwSetCursorPos(window, mov->getLastX(), mov->getLastY());
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}
void configDepht()
{
	glGenFramebuffers(1, &depthMapFBO);

	// create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void renderDepht() 
{
	glm::mat4 lightProjection, lightView;
	//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
	lightProjection = glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, near_plane, far_plane);
    glm::vec3 sunDirection = -1.5f * glm::vec3(userInterface->lightP[0], userInterface->lightP[1], userInterface->lightP[2]);
	lightView = glm::lookAt(sunDirection, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view
	glm::mat4 model = glm::mat4(1.0f);
	depthShader->use();
	depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	depthShader->setMat4("modelLSP", model);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID[0]);
	render(depthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void debugDepht()
{//Render the scene from the camera position.

    debugDepthQuad->use();
    debugDepthQuad->setInt("depthTest", 0);
    debugDepthQuad->setFloat("near_plane", near_plane);
    debugDepthQuad->setFloat("far_plane", far_plane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    render(debugDepthQuad);
}