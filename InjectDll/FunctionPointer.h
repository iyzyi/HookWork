#pragma once
#include "FunctionPointer.h"
#include "FunctionPrototype.h"


extern PFN_Send True_Send = (PFN_Send)GetProcAddress(GetModuleHandleA("ws2_32"), "send");

extern PFN_Recv True_Recv = (PFN_Recv)GetProcAddress(GetModuleHandleA("ws2_32"), "recv");