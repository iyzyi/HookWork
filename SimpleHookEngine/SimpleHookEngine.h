#pragma once

#include <windows.h>

extern BOOL InstallHook(PVOID* ppSystemFunction, PVOID pHookFunction);
extern BOOL UninstallHook(PVOID* ppHookedFunction);