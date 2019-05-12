#include "odbk_def.h"
#include "odbk_core.h"
#include "odbk_hook.h"

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved)
{
  switch (reason)
  {
    case DLL_PROCESS_ATTACH:
#if _DEBUG_DEBUGGER
      MessageBoxA(NULL, "Waiting to attach the debugger", "tips", MB_OK | MB_ICONEXCLAMATION);
#endif
      odbk_hook_init();
      singleton<odbk_core>::instance()->init(hInstance);
      break;
    case DLL_PROCESS_DETACH:
      singleton<odbk_core>::destroy();
      odbk_hook_uninit();
      break;
  }

  return TRUE;
}
