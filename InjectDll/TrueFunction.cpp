#include "pch.h"
#include "TrueFunction.h"



PFN_Send True_Send = (PFN_Send)GetProcAddress(GetModuleHandleA("ws2_32"), "send");

PFN_Recv True_Recv = (PFN_Recv)GetProcAddress(GetModuleHandleA("ws2_32"), "recv");