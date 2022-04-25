#pragma once
#include "TrueFunction.h"


int My_Send(SOCKET s, const char* buf, int len, int flags);

int My_Recv(SOCKET s, char* buf, int len, int flags);