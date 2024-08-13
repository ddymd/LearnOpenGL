#ifndef GL_COMMON_HPP
#define GL_COMMON_HPP

struct GLFWwindow;
class Camera;

void GLFWErrorCB(int ec, const char* emsg);

void GLFWFrameResizeCB(GLFWwindow* window, int w, int h);

void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos);

void GLFWScrollCB(GLFWwindow* window, double xoffset, double yoffset);

void ProcessInputs(GLFWwindow* window, float delta);

void SetCamera(Camera* pcam);

unsigned int LoadTexture(char const* path);

#endif // GL_COMMON_HPP
