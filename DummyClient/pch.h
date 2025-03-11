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
#include "Utils.h"
#include "Enum.pb.h"
