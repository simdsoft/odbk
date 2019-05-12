#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include "yasio/yasio.h"
#include "odbk_internal.h"


using namespace yasio;
using namespace yasio::inet;
using namespace yasio::gc;

enum
{
  APPF_NORMAL,
  APPF_COCOS2DX = 1,
  APPF_UNITY    = 1 << 1,
  APPF_X86      = 1 << 2,
  APPF_X64      = 1 << 3,
};

struct bpinfo
{
    int type; // NORMAL, CONDITION, LOGPOINT
    std::string info; // Can be empty, conditon expression, message
};

class odbk_core
{
public:
  /// Public functions
  ~odbk_core();

  void init(void* hInst);

  void print(int type, const std::string& msg);

  /*
     Send proto data to frontend,
     bp hit, 
     load script, 
     call stack,
     eval result,
     message,
     ...
  */
  void send(std::vector<char> data);

public:
  io_service service_;
  io_transport *vfd_;

  /// Public attributes
  std::string app_root_; // debug target root
  int app_flags_;        // debug target flags

  std::string tools_wr_path_; // AppData\Local\x-studio365\, ldb_predef_modules.csv

  // Windows ONLY
  void* app_inst_; // HANDLE
  void* app_module_; // HMODULE GetModuleHandleW(nullptr)
  void* app_process_; // HANDLE GetCurrentProcess

  // signal for break and wait command
  std::recursive_mutex mtx_;
  std::condition_variable signal_;

private:
  /// Private members
  std::unordered_map<int, std::set<std::string>> bpinfos_;
};

extern "C" {
ODBK_API void odbk_install(void *hModule);
}
