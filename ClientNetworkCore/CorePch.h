#pragma once

#include "CoreTypes.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "Container.h"

#include <windows.h>
#include <iostream>
using namespace std;

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>

#include <boost/asio.hpp>

//#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "mswsock.lib")
//#pragma comment(lib, "advapi32.lib")

#include "Memory.h"
#include "Lock.h"
#include "ObjectPool.h"
#include "TypeCast.h"
#include "SendBuffer.h"
#include "Session.h"
#include "LockQueue.h"
