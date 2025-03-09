#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN


#ifdef _DEBUG
#pragma comment(lib, "ClientNetworkCore\\Debug\\ClientNetworkCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ClientNetworkCore\\Release\\ClientNetworkCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif


#include "CorePch.h"
#include "Types.h"
#include "Utils.h"
#include "Enum.pb.h"
