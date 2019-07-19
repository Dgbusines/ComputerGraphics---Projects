#include "Navigation.h"

navigate::navigate(){
    currentTime = 0;
    deltaTime = lastTime = 0.0f;
    cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    firstMouse = true;

    lastX =  800.0f / 2.0;
    lastY =  600.0 / 2.0;
    fov   =  45.0f;
    pitch = 0.0f;
    yaw = -90.0f;
}

navigate::~navigate(){}

void navigate::movements(GLFWwindow *win){
    //CAMERA BASIC

    float cameraSpeed = 0.25f;
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    viewMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    projectionMat = glm::perspective(glm::radians(fov), 4.0f / 3.0f, 0.1f, 100.0f);

    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    cameraSpeed = 2.5f * deltaTime;

}

glm::mat4 navigate::getViewMatrix() {
    return viewMat;
}

glm::mat4 navigate::getProjectionMatrix() {
    return projectionMat;
}

void navigate::mouse(GLFWwindow * win, double xpos, double ypos) {
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.15f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void navigate::scroll(double xoffset, double yoffset) {
  if(fov >= 1.0f && fov <= 45.0f)
  	fov -= yoffset;
  if(fov <= 1.0f)
  	fov = 1.0f;
  if(fov >= 45.0f)
  	fov = 45.0f;
}

glm::vec3 navigate::getCameraPos() {
    return cameraPos;
}

glm::vec3 navigate::getCameraFront() {
    return cameraFront;
}

double navigate::getLastX() {
    return (double)lastX;
}

double navigate::getLastY() {
    return (double)lastY;
}
