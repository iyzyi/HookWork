#ifdef _M_IX86
#define _M_IX86_X64
#elif defined _M_X64
#define _M_IX86_X64
#endif

BOOL SetHook(PVOID *ppSystemFunction, PVOID pHookFunction);
BOOL UnHook(PVOID *ppHookedFunction);

#define MHOOKS_MAX_SUPPORTED_HOOKS 32768