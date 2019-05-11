#include "odbk_def.h"
#include "odbk_hook.h"
#include "MinHook.h"

#if ODBK_BITS == 32
#  pragma comment(lib, "libMinHook.x86.lib")
#else
#  pragma comment(lib, "libMinHook.x64.lib")
#endif

void odbk_hook_init() { MH_Initialize(); }
void odbk_hook_uninit() { MH_Uninitialize(); }

void *odbk_hook(void **ppOriginal, void *pDetour)
{
  void *pTarget = *ppOriginal;
  if (pTarget != nullptr)
    MH_CreateHook(pTarget, pDetour, ppOriginal);
  return pTarget;
}
void odbk_unhook(void *pTarget) { MH_RemoveHook(pTarget); }

void odbk_enable_hook(void *pTarget) { MH_EnableHook(pTarget); }
void odbk_disable_hook(void *pTarget) { MH_DisableHook(pTarget); }
