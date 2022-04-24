#pragma once

extern BOOL InstallHook(PVOID* ppSystemFunction, PVOID pHookFunction);
extern BOOL UninstallHook(PVOID* ppHookedFunction);