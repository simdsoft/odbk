#include "odbk_lload.h"
#include "odbk_lhook.h"
#include "odbk_core.h"

#include <Windows.h>

odbk_core::odbk_core() {}
odbk_core::~odbk_core() {}

void odbk_core::init(void* hInst)
{
  app_inst_    = hInst;
  app_module_  = GetModuleHandleW(nullptr);
  app_process_ = GetCurrentProcess();
}

void odbk_core::print(const std::string &msg) {}

extern "C" {
ODBK_API void odbk_install(void *hModule)
{
  odbk_load_lua(hModule);
  odbk_install_lua_hooks();
}
}
