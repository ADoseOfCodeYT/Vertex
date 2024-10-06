#pragma once

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "MaterialManager.h"
#include "Model.h"
#include "Math.h"
#include "Level.h"

const char* WindowTitle = "half life 3!!!?!?!?!?!?";

void FrameBufferSizeCallBack(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window); // key presses triggered more than once
void MouseCallBack(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods); // key presses only triggered once

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool MouseLockEnabled = true;

bool IsCtrlDown = false;

Camera GlobalCamera(glm::vec3(0.0f, 0.0f, 3.0f));


