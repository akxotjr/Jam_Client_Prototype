#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN


#ifdef _DEBUG
#pragma comment(lib, "Debug\\ClientNetworkCore.lib")
#else
#pragma comment(lib, "Release\\ClientNetworkCore.lib")
#endif


#include "CorePch.h"
#include "Types.h"
#include "Defines.h"
#include "Values.h"
#include "Utils.h"
#include "Enum.pb.h"
#include "Protocol.pb.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>