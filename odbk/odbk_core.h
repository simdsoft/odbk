#pragma once

#include <string>
#include <vector>
#include "odbk_def.h"
#include "yasio/detail/singleton.h"
using namespace yasio::gc;

enum
{
  APPF_NORMAL,
  APPF_COCOS2DX = 1,
  APPF_UNITY    = 1 << 1,
  APPF_X86      = 1 << 2,
  APPF_X64      = 1 << 3,
};

class odbk_core
{
public:
  /// Public functions
  odbk_core();
  ~odbk_core();

  void init(void* hInst);

  void print(const std::string& msg);

public:
  /// Public attributes
  std::string app_root_; // debug target root
  int app_flags_;        // debug target flags

  std::string tools_wr_path_; // AppData\Local\x-studio365\, ldb_predef_modules.csv

  void* app_inst_; // HANDLE
  void* app_module_; // HMODULE GetModuleHandleW(nullptr)
  void* app_process_; // HANDLE GetCurrentProcess

private:
  /// Private members
};

extern "C" {
ODBK_API void odbk_install(void *hModule);
}
