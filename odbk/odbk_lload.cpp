#include "yasio/ibstream.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <fstream>
#include "odbk_core.h"
#include "odbk_utils.h"
#include "odbk_dbghelp.h"

#include "odbk_lload.h"

extern "C" {
lua_open_func_t __lua_open;
lua_newstate_func_t __lua_newstate;
luaopen_base_func_t __luaopen_base;
lua_close_func_t __lua_close;
lua_newthread_func_t __lua_newthread;
lua_error_func_t __lua_error;
lua_absindex_func_t __lua_absindex;
lua_gettop_func_t __lua_gettop;
lua_sethook_func_t __lua_sethook;
lua_gethookmask_func_t __lua_gethookmask;
lua_getinfo_func_t __lua_getinfo;
lua_remove_func_t __lua_remove;
lua_rotate_func_t __lua_rotate;
lua_settable_func_t __lua_settable;
lua_gettable_func_t __lua_gettable;
lua_setglobal_func_t __lua_setglobal;
lua_getglobal_func_t __lua_getglobal;
lua_rawget_func_t __lua_rawget;
lua_rawgeti_func_t __lua_rawgeti;
lua_rawset_func_t __lua_rawset;
lua_rawseti_func_t __lua_rawseti;
lua_pushstring_func_t __lua_pushstring;
lua_pushlstring_func_t __lua_pushlstring;
lua_type_func_t __lua_type;
lua_typename_func_t __lua_typename;
lua_settop_func_t __lua_settop;
lua_getlocal_func_t __lua_getlocal;
lua_setlocal_func_t __lua_setlocal;
lua_getstack_func_t __lua_getstack;
lua_insert_func_t __lua_insert;
lua_pushnil_func_t __lua_pushnil;
lua_pushvalue_func_t __lua_pushvalue;
lua_pushinteger_func_t __lua_pushinteger;
lua_pushnumber_func_t __lua_pushnumber;
lua_pushboolean_func_t __lua_pushboolean;
lua_pushcclosure_func_t __lua_pushcclosure;
lua_tostring_func_t __lua_tostring;
lua_tolstring_func_t __lua_tolstring;
lua_toboolean_func_t __lua_toboolean;
lua_isinteger_func_t __lua_isinteger;
lua_tointeger_func_t __lua_tointeger;
lua_tointegerx_func_t __lua_tointegerx;
lua_tocfunction_func_t __lua_tocfunction;
lua_tonumber_func_t __lua_tonumber;
lua_tonumberx_func_t __lua_tonumberx;
lua_touserdata_func_t __lua_touserdata;
lua_load_510_func_t __lua_load_510;
lua_load_func_t __lua_load;
lua_load_func_t __lua_loadx;
lua_call_func_t __lua_call;
lua_callk_func_t __lua_callk;
lua_pcall_func_t __lua_pcall;
lua_pcallk_func_t __lua_pcallk;
lua_newtable_func_t __lua_newtable;
lua_createtable_func_t __lua_createtable;
lua_next_func_t __lua_next;
lua_rawequal_func_t __lua_rawequal;
lua_getmetatable_func_t __lua_getmetatable;
lua_setmetatable_func_t __lua_setmetatable;
luaL_ref_func_t __luaL_ref;
luaL_unref_func_t __luaL_unref;
luaL_newmetatable_func_t __luaL_newmetatable;
luaL_loadbuffer_func_t __luaL_loadbuffer;
luaL_loadbufferx_func_t __luaL_loadbufferx;
luaL_loadfile_func_t __luaL_loadfile;
luaL_loadfilex_func_t __luaL_loadfilex;
luaL_register_func_t __luaL_register; // halx99
luaL_openlibs_func_t __luaL_openlibs; // halx99
luaI_openlib_func_t __luaI_openlib;   // halx99
lua_towstring_func_t __lua_towstring;
lua_iswstring_func_t __lua_iswstring;
lua_getupvalue_func_t __lua_getupvalue;
lua_setupvalue_func_t __lua_setupvalue;
lua_getfenv_func_t __lua_getfenv;
lua_setfenv_func_t __lua_setfenv;
lua_pushlightuserdata_func_t l__ua_pushlightuserdata;
lua_cpcall_func_t __lua_cpcall;
lua_pushthread_func_t __lua_pushthread;
lua_newuserdata_func_t __lua_newuserdata;
luaL_newstate_func_t __luaL_newstate;
lua_checkstack_func_t __lua_checkstack;
lua_concat_func_t __lua_concat;
luaL_where_func_t __luaL_where;

struct odbk_lua_context
{
  bool loaded = false;
  std::vector<std::string> specified_modules;
  std::unordered_map<std::string, uintptr_t> symbols;
  std::unordered_set<std::string> restricts;
  HMODULE key_module = nullptr; // the lua vm dll.
  std::string key_module_name;

  ///////////////////////////
  int version = 510;
  int registry_index;
  int globals_index;
  int hook_tail_call;
  int hook_tail_ret;
};
static odbk_lua_context *s_ctx;

// ---------------------- compact functions for ----------------------------
lua_Integer(__lua_tointeger__compact)(lua_State *L, int idx)
{
  return lua_tointegerx(L, idx, NULL);
}

// ------------- 32bits thunks for support lua-5.3.x -----------------------
#if ODBK_BITS == 32
union lua_LargeInteger
{
  struct
  {
    uint32_t LowPart;
    int32_t HighPart;
  } DUMMYSTRUCTNAME;
  struct
  {
    uint32_t LowPart;
    int32_t HighPart;
  } u;
  int64_t QuadPart;
};
#  if defined(_WIN32)
void(__lua_rawgeti__thunk)(lua_State *L, int idx, lua_Integer n)
{
  lua_LargeInteger li;
  li.QuadPart      = n;
  auto lua_version = s_ctx->version;
  __asm {
		cmp lua_version, 530;
		jge L_GEQ_530;
    // L_LSS_530:
		push li.LowPart;
		push idx;
		push L;
		call __lua_rawgeti;
		add esp, 12
	    jmp L_RET;
	L_GEQ_530:
		push li.HighPart;
		push li.LowPart;
		push idx;
		push L;
		call __lua_rawgeti;
		add esp, 16
	L_RET: ;
  }
}
void(__lua_rawseti__thunk)(lua_State *L, int idx, lua_Integer n)
{
  lua_LargeInteger li;
  li.QuadPart      = n;
  auto lua_version = s_ctx->version;
  __asm {
		cmp lua_version, 530;
		jge L_GEQ_530;
    // L_LSS_530:
		push li.LowPart;
		push idx;
		push L;
		call __lua_rawseti;
		add esp, 12
	    jmp L_RET;
	L_GEQ_530:
		push li.HighPart;
		push li.LowPart;
		push idx;
		push L;
		call __lua_rawseti;
		add esp, 16
	L_RET: ;
  }
}
void(__lua_pushinteger__thunk)(lua_State *L, lua_Integer n)
{
  lua_LargeInteger li;
  li.QuadPart      = n;
  auto lua_version = s_ctx->version;
  __asm {
		cmp lua_version, 530;
		jge L_GEQ_530;
    // L_LSS_530:
		push li.LowPart;
		push L;
		call __lua_pushinteger;
		add esp, 8
	    jmp L_RET;
	L_GEQ_530:
		push li.HighPart;
		push li.LowPart;
		push L;
		call __lua_pushinteger;
		add esp, 12
	L_RET: ;
  }
}
lua_Integer(__lua_tointegerx__thunk)(lua_State *L, int idx, int *isnum)
{
  lua_LargeInteger li;
  li.QuadPart      = 0;
  auto lua_version = s_ctx->version;
  __asm {
		push isnum;
		push idx;
		push L;
		call __lua_tointegerx;
		add esp, 12
		mov dword ptr[li.LowPart], eax;
		cmp lua_version, 530;
		jge L_GEQ_530;
    // L_LSS_530:
		jmp L_RET;
	L_GEQ_530:
		mov dword ptr[li.HighPart], edx;
		
	L_RET: ;
  }
  return li.QuadPart;
}
#  endif
#endif

// --------------------  TODO: try load All lua APIs ----------------------
#define USE_SLIB_WARN_MSG                                                                          \
  "Warning: The lua engine dynamic library not found, it maybe take more than 7 seconds to start " \
  "debug at first time after the target .exe changed!\n"                                           \
  "*** Suggest use dynamic lua engine library, then:\n"                                            \
  "1.You can start debug fastly at anytime\n"                                                      \
  "2.You can debug release mode exe without .pdb\n"

static bool load_specified_modules(odbk_core *core);
static void load_sym_restricts(odbk_core *core);
static bool load_sym_from_cache(odbk_core *core);
static void load_sym_from_app_modules(odbk_core *core);
static bool odbk_lua_finish_load(std::unordered_map<std::string, uintptr_t> &symols);

bool odbk_load_lua(void *hLua)
{
  return odbk_load_lua_internal(singleton<odbk_core>::instance(), hLua);
}
bool odbk_load_lua_internal(odbk_core *core, void *hLua)
{
  if (!s_ctx)
    s_ctx = new odbk_lua_context();

  if (hLua == nullptr)
  {
    if (!load_specified_modules(core))
    { // use builtin module name
      s_ctx->specified_modules = std::vector<std::string>(
          {"lua51.dll", "lua52.dll", "lua53.dll", "liblua51.dll", "liblua52.dll", "liblua53.dll",
           "slua.dll", "ulua.dll", "tolua.dll", "xlua.dll"});
    }

    // try load lua api from specified modules
    for (auto &moduleName : s_ctx->specified_modules)
    {
      auto handle = GetModuleHandleA(moduleName.c_str());
      if (handle)
      {
        odbk_get_exports(handle, s_ctx->symbols);
        break;
      }
    }

    if (s_ctx->symbols.empty())
    {
      // print warning message
      ODBK_LOGWARN("%s", USE_SLIB_WARN_MSG);

      // try load from cache
      if (!load_sym_from_cache(core))
      {
        if (s_ctx->restricts.empty())
          load_sym_restricts(core);

        // try load from all modules
        load_sym_from_app_modules(core);

        if (!s_ctx->symbols.empty())
          ; // TODO: cache symbols va
      }
    }
  }
  else
  { // for delay load
    s_ctx->symbols.clear();
    odbk_get_exports((HMODULE)hLua, s_ctx->symbols);
  }

  return odbk_lua_finish_load(s_ctx->symbols);
}

int odbk_lua_version() { return s_ctx->version; }

static void *get_lua_fn(const char *name)
{
  auto it = s_ctx->symbols.find(name);
  if (it != s_ctx->symbols.end())
    return (void *)it->second;

  return nullptr;
}

static bool odbk_lua_finish_load(std::unordered_map<std::string, uintptr_t> &symbols)
{
  if (symbols.find("lua_open") != symbols.end())
  { // ONLY lua 5.0.x has this function.
    s_ctx->version        = 500;
    s_ctx->registry_index = -10000;
    s_ctx->globals_index  = -10001;
  }
  else if (symbols.find("lua_rotate") != symbols.end())
  {
    s_ctx->version = 530;
    // LUA_REGISTRYINDEX == LUAI_FIRSTPSEUDOIDX with LUAI_FIRSTPSEUDOIDX == (-LUAI_MAXSTACK -
    // 1000) with LUAI_MAXSTACK == 15000 (for 32 bits build...)
    s_ctx->registry_index = -1001000;
    // starting with Lua 5.2, there is no longer a LUA_GLOBALSINDEX pseudo-index. Instead the
    // global table is stored in the registry at LUA_RIDX_GLOBALS
    s_ctx->globals_index  = 2;
    s_ctx->hook_tail_call = LUA_HOOKTAILCALL; // Lua5.2 has LUA_HOOKTAILCALL, but no LUA_HOOKTAILRET
  }
  else if (symbols.find("lua_callk") != symbols.end())
  {
    s_ctx->version = 520;
    // LUA_REGISTRYINDEX == LUAI_FIRSTPSEUDOIDX with LUAI_FIRSTPSEUDOIDX == (-LUAI_MAXSTACK -
    // 1000) with LUAI_MAXSTACK == 15000 (for 32 bits build...)
    s_ctx->registry_index = -1001000;
    // starting with Lua 5.2, there is no longer a LUA_GLOBALSINDEX pseudo-index. Instead the
    // global table is stored in the registry at LUA_RIDX_GLOBALS
    s_ctx->globals_index  = 2;
    s_ctx->hook_tail_call = LUA_HOOKTAILCALL; // Lua5.2 has LUA_HOOKTAILCALL, but no LUA_HOOKTAILRET
  }
  else if (symbols.find("lua_gettop") != symbols.end()) // should be ok for any version
  {
    s_ctx->version        = 510;
    s_ctx->registry_index = -10000;
    s_ctx->globals_index  = -10002;
    // Lua5.1 has LUA_HOOKTAILRET, but no LUA_HOOKTAILCALL
    s_ctx->hook_tail_call = LUA_HOOKTAILRET;
  }
  else // if we get here, this means the module isn't related to Lua at all
  {
    return false;
  }

#define GET_FUNCTION(function)                                                                     \
  __##function = reinterpret_cast<decltype(__##function)>(get_lua_fn(#function));                  \
  if (__##function == nullptr)                                                                     \
  {                                                                                                \
    ODBK_LOGERR("%s", "Warning 1004: Couldn't Get Lua function '" #function "'");                  \
    return false;                                                                                  \
  }                                                                                                \
  (void)0

#define GET_FUNCTION_OPTIONAL(function)                                                            \
  __##function = reinterpret_cast<decltype(__##function)>(get_lua_fn(#function))

#define GET_FUNCTION_OPTIONAL_ALIAS(function, alias)                                               \
  __##function = reinterpret_cast<decltype(__##function)>(get_lua_fn(#function));                  \
  if (__##function == nullptr)                                                                     \
    __##function = reinterpret_cast<decltype(__##function)>(get_lua_fn(#alias));                   \
  (void)0

#define GET_FUNCTION_OPTIONAL_RELOCATE(function, alias)                                            \
  __##function = reinterpret_cast<decltype(__##function)>(get_lua_fn(#alias))

  // Only present in Lua 4.0 and Lua 5.0 (not 5.1)
  GET_FUNCTION_OPTIONAL(lua_open);
  if (__lua_open == nullptr)
  {
    GET_FUNCTION(lua_newstate);
  }

  GET_FUNCTION(lua_newthread);
  GET_FUNCTION(lua_close);
  GET_FUNCTION(lua_error);
  GET_FUNCTION(luaopen_base);
  GET_FUNCTION_OPTIONAL(lua_absindex); // Only present in Lua 5.2+
  GET_FUNCTION(lua_sethook);
  GET_FUNCTION(lua_getinfo);
  GET_FUNCTION_OPTIONAL(lua_remove); // Only present up to Lua 5.3
  GET_FUNCTION_OPTIONAL(lua_rotate); // Only present in Lua 5.3
  GET_FUNCTION(lua_settable);
  GET_FUNCTION(lua_gettable);
  GET_FUNCTION(lua_rawget);
  GET_FUNCTION(lua_rawgeti);
  GET_FUNCTION(lua_rawset);
  GET_FUNCTION(lua_pushstring);
  GET_FUNCTION(lua_pushlstring);
  GET_FUNCTION(lua_type);
  GET_FUNCTION(lua_typename);
  GET_FUNCTION(lua_settop);
  GET_FUNCTION(lua_gettop);
  GET_FUNCTION(lua_getlocal);
  GET_FUNCTION(lua_setlocal);
  GET_FUNCTION(lua_getstack);
  GET_FUNCTION_OPTIONAL(lua_insert); // Only present in Lua < 5.3
  GET_FUNCTION(lua_pushnil);
  GET_FUNCTION(lua_pushvalue);
  GET_FUNCTION(lua_pushcclosure);
  GET_FUNCTION(lua_pushnumber);
  // GET_FUNCTION(lua_pushlightuserdata);
  GET_FUNCTION(lua_checkstack);
  GET_FUNCTION(lua_gethookmask);
  GET_FUNCTION(lua_pushboolean);

  GET_FUNCTION_OPTIONAL(lua_concat);
  GET_FUNCTION_OPTIONAL(luaL_where);

  // Only present in Lua 5.2+
  GET_FUNCTION_OPTIONAL(lua_getglobal);
  GET_FUNCTION_OPTIONAL(lua_setglobal);

  // Only present in Lua 5.1 (*number funtions used in Lua 4.0)
  GET_FUNCTION_OPTIONAL(lua_pushinteger);
  GET_FUNCTION_OPTIONAL(lua_tointeger);
  GET_FUNCTION_OPTIONAL(lua_tointegerx);

  // Only present in Lua 4.0 and 5.0 (exists as a macro in Lua 5.1)
  GET_FUNCTION_OPTIONAL(lua_tostring);

  if (__lua_tostring == nullptr)
  {
    GET_FUNCTION(lua_tolstring);
  }

  GET_FUNCTION_OPTIONAL(lua_tonumberx);

  // If the Lua 5.2 tonumber isn't present, require the previous version.
  if (__lua_tonumberx == nullptr)
  {
    GET_FUNCTION(lua_tonumber);
  }

  GET_FUNCTION(lua_toboolean);
  GET_FUNCTION(lua_tocfunction);
  GET_FUNCTION(lua_touserdata);

  // Exists as a macro in Lua 5.2
  GET_FUNCTION_OPTIONAL(lua_callk);
  if (__lua_callk == nullptr)
  {
    GET_FUNCTION(lua_call);
  }

  // Exists as a macro in Lua 5.2
  GET_FUNCTION_OPTIONAL(lua_pcallk);
  if (__lua_pcallk == NULL)
  {
    GET_FUNCTION(lua_pcall);
  }

  // Only present in Lua 4.0 and 5.0 (exists as a macro in Lua 5.1)
  GET_FUNCTION_OPTIONAL(lua_newtable);
  if (__lua_newtable == NULL)
  {
    GET_FUNCTION(lua_createtable);
  }

  // TODO: check load dependency infomrations.
  // Some static link project maybe no this function on .pdb, so make it optional
  GET_FUNCTION_OPTIONAL(lua_load);
  GET_FUNCTION(lua_next);
  GET_FUNCTION(lua_rawequal);
  GET_FUNCTION(lua_getmetatable);
  GET_FUNCTION(lua_setmetatable);
  GET_FUNCTION_OPTIONAL(luaL_ref);
  GET_FUNCTION_OPTIONAL(luaL_unref);
  GET_FUNCTION(luaL_newmetatable);
  GET_FUNCTION(lua_getupvalue);
  GET_FUNCTION(lua_setupvalue);

  GET_FUNCTION(luaL_openlibs); // open all base libs

  // Only available in Lua 5.0/5.1
  GET_FUNCTION_OPTIONAL(lua_getfenv);
  GET_FUNCTION_OPTIONAL(lua_setfenv);
  GET_FUNCTION_OPTIONAL(lua_cpcall);

  GET_FUNCTION(lua_newuserdata);

  // This function isn't strictly necessary. We only hook it
  // in case the base function was inlined.
  GET_FUNCTION_OPTIONAL(luaL_newstate);

  GET_FUNCTION_OPTIONAL(luaL_register);
  // GET_FUNCTION_ALIAS(luaI_openlib, luaL_openlib);
  // Lua53 no this function,TODO: check 51,52, not jit.
  GET_FUNCTION_OPTIONAL_ALIAS(luaI_openlib, luaL_openlib);

  GET_FUNCTION_OPTIONAL(luaL_loadbuffer);
  GET_FUNCTION_OPTIONAL(luaL_loadbufferx); // 5.2 or Later
  GET_FUNCTION_OPTIONAL(luaL_loadfile);
  GET_FUNCTION_OPTIONAL(luaL_loadfilex);

  // These functions only exists in LuaPlus.
  GET_FUNCTION_OPTIONAL(lua_towstring);
  GET_FUNCTION_OPTIONAL(lua_iswstring);

  // Hook the functions we need to intercept calls to.

  if (s_ctx->version >= 510)
  {
    GET_FUNCTION(lua_pushthread);
  }
  else
  { // This function doesn't exist in Lua 5.0, so make it optional.
    GET_FUNCTION_OPTIONAL(lua_pushthread);
  }

  // Lua 5.3 64 bit integer support
  GET_FUNCTION_OPTIONAL(lua_isinteger);

  // Try Get luajit lua_loadx
  GET_FUNCTION_OPTIONAL(lua_loadx);
  GET_FUNCTION_OPTIONAL_RELOCATE(lua_load, lua_loadx);

  return true;
}

static bool load_specified_modules(odbk_core *core)
{ // TODO: implementation, support config
  if (!s_ctx->specified_modules.empty())
    return true;
  return false;
}

static bool load_sym_from_cache(odbk_core *core) { return false; }

static void load_sym_restricts(odbk_core *core)
{
  std::ifstream fin;
  // fin.open(strLuaAPIPath);
  return;

  fin.seekg(0, std::ios_base::end);
  std::streamsize bytes_left = fin.tellg(); // filesize initialized
  fin.seekg(0, std::ios_base::beg);

  std::string data(bytes_left, '\0');
  fin.read(&data.front(), bytes_left);
  fin.close();

  yasio::ibstream_view ibs(data.c_str(), data.size());
  auto count = 0;
  ibs.read_ix(count);

  std::vector<std::string> checks;
  for (auto i = 0; i < count; ++i)
  {
    std::string func;
    ibs.read_v16(func); // fix issue: https://github.com/halx99/x-studio365/issues/51
    s_ctx->restricts.insert(std::move(func));
  }
}

struct load_sym_context
{
  std::unordered_set<HANDLE> loaded_modules;
  std::unordered_set<HANDLE> warned_modules;
};
static BOOL CALLBACK gather_symbols_callback(PSYMBOL_INFO pSymInfo, ULONG SymbolSize,
                                             PVOID UserContext)
{
  auto &restricts = s_ctx->restricts;
  auto &symbols   = s_ctx->symbols;
  if (pSymInfo != NULL && pSymInfo->Name != NULL)
  {
    if (restricts.find(pSymInfo->Name) != restricts.end())
    {
      auto symbolIt = symbols.find(pSymInfo->Name);
      if (symbolIt == symbols.end())
        symbols.emplace(pSymInfo->Name, pSymInfo->Address);
      else
      {   // TODO: Optimize me, use a list of module:symbols, ignore
        ; //  symbolIt->second = pSymInfo->Address;
      }
    }
  }

  return TRUE;
}
static void load_sym_from_module(HMODULE hModule, odbk_core *core, load_sym_context *context)
{
  if (hModule == core->app_inst_)
    return;
  if (context->loaded_modules.find(hModule) != context->loaded_modules.end())
    return;

  auto hProcess = core->app_process_;

  char moduleName[_MAX_PATH];
  GetModuleBaseNameA(hProcess, hModule, moduleName, _MAX_PATH);

  // check whether luaxx.dll by signature or load all symbols required
  // Record that we've loaded this module so that we don't
  // try to load it again.
  context->loaded_modules.insert(hModule);

  MODULEINFO moduleInfo = {0};
  GetModuleInformation(hProcess, hModule, &moduleInfo, sizeof(moduleInfo));

  char moduleFileName[_MAX_PATH];
  int moduleFileNameLen = GetModuleFileNameExA(hProcess, hModule, moduleFileName, _MAX_PATH);

  DWORD64 base = odbk::SymLoadModule64(hProcess, NULL, moduleFileName, moduleName,
                                       (DWORD64)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage);

  // Check to see if there was a symbol file we failed to load (usually
  // becase it didn't match the version of the module).
  IMAGEHLP_MODULE64 module;
  memset(&module, 0, sizeof(module));
  module.SizeOfStruct = sizeof(module);

  BOOL result = odbk::SymGetModuleInfo64(hProcess, base, &module);

  if (result && module.SymType == SymNone)
  {
    // No symbols were found. Check to see if the module file name + ".pdb"
    // exists, since the symbol file and/or module names may have been renamed.
    char pdbFileName[_MAX_PATH];
    strcpy(pdbFileName, moduleFileName);
    odbk_replace_extension(pdbFileName, moduleFileNameLen, "pdb");

    if (odbk_file_exists(pdbFileName))
    {

      odbk::SymUnloadModule64(hProcess, base);
      base = odbk::SymLoadModule64(hProcess, NULL, pdbFileName, moduleName,
                                   (DWORD64)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage);

      if (base != 0)
      {
        result = odbk::SymGetModuleInfo64(hProcess, base, &module);
      }
      else
      {
        result = FALSE;
      }
    }
  }

  if (result)
  {
    // Check to see if we've already warned about this module.
    if (*module.CVData != '\0' && (module.SymType == SymExport || module.SymType == SymNone))
    {
      if (context->warned_modules.find(hModule) == context->warned_modules.end())
      {
        char symbolFileName[_MAX_PATH];

        if (odbk::locate_sym_file(module, symbolFileName))
        {
          ODBK_LOGWARN("Warning 1002: Symbol file '%s' located but it does not match module '%s'",
                       symbolFileName, moduleFileName);
        }

        // Remember that we've checked on this file, so no need to check again.
        context->warned_modules.insert(moduleFileName);
      }
    }
  }

  bool foundLuaFunctions = false;

  if (base != 0)
  {
    // SymFromName is really slow, so we gather up our own list of the symbols that we
    // can index much faster.
    auto currentSize = s_ctx->symbols.size();
    odbk::SymEnumSymbols(hProcess, base, "lua*", gather_symbols_callback, nullptr);
    foundLuaFunctions = currentSize < s_ctx->symbols.size();
    if (foundLuaFunctions)
    {
      if (s_ctx->key_module == nullptr || hModule != core->app_module_)
      { // setup key module
        s_ctx->key_module      = hModule;
        s_ctx->key_module_name = moduleName;
      }
    }
  }

  if (foundLuaFunctions)
  { // !!!@!!!Remark: The address of functions lua always same, when binary(.exe) not changed, so
    // we can cache the lua symbols for startup debugging fast.
    // halx99, for startup speed, default load lua**.dll symbols only.
    ODBK_LOGINFO("Lua exports API found at '%s'. Symbols loaded", moduleFileName);

    if (s_ctx->symbols.size() >= 166)
    { // return if not load all symbols, g_lua5xAPISet.size() = 172, lua51~lua53 All exports API
      // count is: 172
      ODBK_LOGINFO("%s", "Load lua symbols only required, skip all other module!");
      return;
    }
  }

  // Get the imports for the module. These are loaded before we're able to hook
  // LoadLibrary for the module.
  std::vector<std::string> imports;
  odbk_get_imports(hModule, imports);

  for (unsigned int i = 0; i < imports.size(); ++i)
  {
    HMODULE hImportModule = GetModuleHandleA(imports[i].c_str());

    // Sometimes the import module comes back NULL, which means that for some reason
    // it wasn't loaded. Perhaps these are delay loaded and we'll catch them later?
    if (hImportModule != NULL)
    {
      load_sym_from_module(hImportModule, core, context);
    }
  }
}

static void load_sym_from_app_modules(odbk_core *core)
{
  ULONG cookie = 0;
  odbk::LdrLockLoaderLock(0, 0, &cookie);

  load_sym_context context;

  // Process all of the loaded modules.
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
  if (hSnapshot != nullptr)
  {
    MODULEENTRY32 module_entry;
    module_entry.dwSize = sizeof(MODULEENTRY32);

    BOOL moreModules = Module32First(hSnapshot, &module_entry);

    while (moreModules)
    {
      load_sym_from_module(module_entry.hModule, core, &context);
      moreModules = Module32Next(hSnapshot, &module_entry);
    }

    CloseHandle(hSnapshot);
  }
  else
  {
    load_sym_from_module((HMODULE)core->app_module_, core, &context);
  }

  odbk::LdrUnlockLoaderLock(0, cookie);
}
}
