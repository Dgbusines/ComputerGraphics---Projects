//#include <glad/glad.h> // Glad has to be include before glfw
//#include <GL/glew.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"

class navigate{
    private:
        glm::mat4 projectionMat;
        glm::mat4 viewMat;
        double currentTime;
        float deltaTime, lastTime, fov, lastX, lastY, pitch, yaw;
        glm::vec3 cameraPos; //  = glm::vec3(0.0f, 0.0f,  3.0f);
        glm::vec3 cameraFront; //= glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp;    //= glm::vec3(0.0f, 1.0f,  0.0f);
        bool firstMouse;
    public:
        navigate();
        ~navigate();
        void movements(GLFWwindow *win);
        glm::mat4 getProjectionMatrix();
        glm::mat4 getViewMatrix();
        glm::vec3 getCameraPos();
        glm::vec3 getCameraFront();
        void mouse(GLFWwindow * win, double xpos, double ypos);
        void scroll(double xoffset, double yoffset);
        double getLastX ();
        double getLastY ();
};

