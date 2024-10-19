#pragma once

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <iostream>

#include "../Renderer/Renderer.h"
#include "../Level/Camera.h"
#include "../Level/MaterialManager.h"
#include "../Level/Model.h"
#include "../Math/Math.h"
#include "../Level/Level.h"

const char* WindowTitle = "Vertex";

void FrameBufferSizeCallBack(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window); // key presses triggered more than once
void MouseCallBack(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods); // key presses only triggered once


float DeltaTime = 0.0f;
float LastFrame = 0.0f;
unsigned int FrameCount = 0;

//Measuring FPS and stuff

std::string Metrics;
std::string MS;
std::string FPS;

bool MouseLockEnabled = true;
bool IsCtrlDown = false;

Camera GlobalCamera(glm::vec3(0.0f, 0.0f, 3.0f)); // create a camera


