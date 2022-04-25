#pragma once
#include <WinSock2.h>


// *******************************************************************************************

typedef int (WINAPI* PFN_Send)(SOCKET s, const char* buf, int len, int flags);

typedef int (WINAPI* PFN_Recv)(SOCKET s, char* buf, int len, int flags);








// *******************************************************************************************

extern PFN_Send True_Send;

extern PFN_Recv True_Recv;