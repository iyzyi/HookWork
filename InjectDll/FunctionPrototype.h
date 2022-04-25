#pragma once
#include "FunctionPrototype.h"
#include <WinSock2.h>


typedef int (WINAPI* PFN_Send)(SOCKET s, const char* buf, int len, int flags);

typedef int (WINAPI* PFN_Recv)(SOCKET s, char* buf, int len, int flags);