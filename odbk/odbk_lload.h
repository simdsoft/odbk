#pragma once
#include <stdlib.h>
#include <stdint.h>
#include "odbk_internal.h"

/*
pitfall-1:
default configuration for 64-bit Lua ('long long' and 'double')
#if !defined(LUA_INT_TYPE)
#  define LUA_INT_TYPE LUA_INT_LONGLONG
#endif

#if !defined(LUA_FLOAT_TYPE)
#  define LUA_FLOAT_TYPE LUA_FLOAT_DOUBLE
#endif

pitfall-2: Unity3D tolua coroutine can't be finialize by it's super state.
 so debugger should create link for it.

pitfall-3:
 lua5.1 load function prototype is different with lua5.2 or later
 we don't define lua5.1 load, just call it at run time
*/

extern "C" {
// comes from luaconf.h, must match the configuration of the VM being debugged
#define LUA_IDSIZE 60

// ===========================
// same in Lua 5.1 and Lua 5.2
// ===========================

// comes from lauxlib.h
#define LUA_NOREF (-2)
#define LUA_REFNIL (-1)

#define LUA_MULTRET (-1)

#define LUA_HOOKCALL 0
#define LUA_HOOKRET 1
#define LUA_HOOKLINE 2
#define LUA_HOOKCOUNT 3
#define LUA_HOOKTAILRET 4  // absent from Lua 5.2
#define LUA_HOOKTAILCALL 4 // Lua 5.2 specific

#define LUA_MASKCALL (1 << LUA_HOOKCALL)
#define LUA_MASKRET (1 << LUA_HOOKRET)
#define LUA_MASKLINE (1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT (1 << LUA_HOOKCOUNT)

#define LUA_TNIL 0
#define LUA_TBOOLEAN 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER 3
#define LUA_TSTRING 4
#define LUA_TTABLE 5
#define LUA_TFUNCTION 6
#define LUA_TUSERDATA 7
#define LUA_TTHREAD 8

#define LUA_OK 0 // Since Lua 5.2
#define LUA_YIELD 1
#define LUA_ERRRUN 2
#define LUA_ERRSYNTAX 3
#define LUA_ERRMEM 4
#define LUA_ERRGCMM 5 // Since Lua 5.2
// LUA_ERRERR	is 5 in Lua 5.1, but 6 in Lua 5.2
#define luaL_reg luaL_Reg

// the lua_Debug structure changes between Lua 5.1 and Lua 5.2
struct lua_Debug_51
{
  int event;
  const char *name;           /* (n) */
  const char *namewhat;       /* (n) `global', `local', `field', `method' */
  const char *what;           /* (S) `Lua', `C', `main', `tail' */
  const char *source;         /* (S) */
  int currentline;            /* (l) */
  int nups;                   /* (u) number of upvalues */
  int linedefined;            /* (S) */
  int lastlinedefined;        /* (S) */
  char short_src[LUA_IDSIZE]; /* (S) */
  /* private part */
  int i_ci; /* active function */
};

// lua5.2 or lua 5.3
struct lua_Debug_52_53
{
  int event;
  const char *name;           /* (n) */
  const char *namewhat;       /* (n) 'global', 'local', 'field', 'method' */
  const char *what;           /* (S) 'Lua', 'C', 'main', 'tail' */
  const char *source;         /* (S) */
  int currentline;            /* (l) */
  int linedefined;            /* (S) */
  int lastlinedefined;        /* (S) */
  unsigned char nups;         /* (u) number of upvalues */
  unsigned char nparams;      /* (u) number of parameters */
  char isvararg;              /* (u) */
  char istailcall;            /* (t) */
  char short_src[LUA_IDSIZE]; /* (S) */
  /* private part */
  struct CallInfo *i_ci; /* active function */
};
 
struct lua_Debug_54 {
   int event;
   const char* name;	/* (n) */
   const char* namewhat;	/* (n) 'global', 'local', 'field', 'method' */
   const char* what;	/* (S) 'Lua', 'C', 'main', 'tail' */
   const char* source;	/* (S) */
   size_t srclen;	/* (S) since lua54 */
   int currentline;	/* (l) */
   int linedefined;	/* (S) */
   int lastlinedefined;	/* (S) */
   unsigned char nups;	/* (u) number of upvalues */
   unsigned char nparams;/* (u) number of parameters */
   char isvararg;        /* (u) */
   char istailcall;	/* (t) */
   unsigned short ftransfer;   /* (r) index of first value transferred since lua54 */
   unsigned short ntransfer;   /* (r) number of transferred values since lua54 */
   char short_src[LUA_IDSIZE]; /* (S) */
   /* private part */
   struct CallInfo* i_ci;  /* active function */
};
 
// we can get LUA_IDSIZE of VM dynamic via SEH
void* odbk_lua_Debug_alloca();

// =====================================================
// must match the configuration of the VM being debugged
// =====================================================

typedef double lua_Number;

// We allways define lua_Integer as 64 bits
// We use the thunk technology to solve it
// For MSVC,GCC,clang 32bits App, use ASM, 64bits application, no need to do it
// Firstly, we implement win32
typedef int64_t lua_Integer;

// !important,
// Lua 5.3 32bit, lua_Integer is longlong, API call will lead crash
// such as lua_istable API.
// if (lua_istable(api, L, glb)) { // #FIXME: Lua5.3 not a table, lead crash, The reason is lua53
// lua_Integer default is long long
//

struct lua_State;
typedef void *(*lua_Alloc)(void *ud, void *ptr, size_t osize, size_t nsize);
typedef void (*lua_Hook)(lua_State *L, lua_Debug *ar);
typedef int (*lua_CFunction)(lua_State *L);
typedef const char *(*lua_Reader)(lua_State *L, void *ud, size_t *sz);

typedef struct luaL_Reg
{
  const char *name;
  lua_CFunction func;
} luaL_Reg;

#define lua_open ODBK_GET_FUN(__lua_open)
#define lua_newstate ODBK_GET_FUN(__lua_newstate)
#define luaopen_base ODBK_GET_FUN(__luaopen_base)
#define lua_close ODBK_GET_FUN(__lua_close)
#define lua_newthread ODBK_GET_FUN(__lua_newthread)
#define lua_error ODBK_GET_FUN(__lua_error)
#define lua_absindex ODBK_GET_FUN(__lua_absindex)
#define lua_gettop ODBK_GET_FUN(__lua_gettop)
#define lua_sethook ODBK_GET_FUN(__lua_sethook)
#define lua_gethookmask ODBK_GET_FUN(__lua_gethookmask)
#define lua_getinfo ODBK_GET_FUN(__lua_getinfo)
#define lua_remove ODBK_GET_FUN(__lua_remove)
#define lua_rotate ODBK_GET_FUN(__lua_rotate)
#define lua_settable ODBK_GET_FUN(__lua_settable)
#define lua_gettable ODBK_GET_FUN(__lua_gettable)
#define lua_setglobal ODBK_GET_FUN(__lua_setglobal)
#define lua_getglobal ODBK_GET_FUN(__lua_getglobal)
#define lua_rawget ODBK_GET_FUN(__lua_rawget)
#define lua_rawgeti ODBK_GET_FUN_THUNK(__lua_rawgeti)
#define lua_rawset ODBK_GET_FUN(__lua_rawset)
#define lua_rawseti ODBK_GET_FUN_THUNK(__lua_rawseti)
#define lua_pushstring ODBK_GET_FUN(__lua_pushstring)
#define lua_pushlstring ODBK_GET_FUN(__lua_pushlstring)
#define lua_type ODBK_GET_FUN(__lua_type)
#define lua_typename ODBK_GET_FUN(__lua_typename)
#define lua_settop ODBK_GET_FUN(__lua_settop)
#define lua_getlocal ODBK_GET_FUN(__lua_getlocal)
#define lua_setlocal ODBK_GET_FUN(__lua_setlocal)
#define lua_getstack ODBK_GET_FUN(__lua_getstack)
#define lua_insert ODBK_GET_FUN(__lua_insert)
#define lua_pushnil ODBK_GET_FUN(__lua_pushnil)
#define lua_pushvalue ODBK_GET_FUN(__lua_pushvalue)
#define lua_pushinteger ODBK_GET_FUN_THUNK(__lua_pushinteger)
#define lua_pushnumber ODBK_GET_FUN(__lua_pushnumber)
#define lua_pushboolean ODBK_GET_FUN(__lua_pushboolean)
#define lua_pushcclosure ODBK_GET_FUN(__lua_pushcclosure)
#define lua_tostring ODBK_GET_FUN(__lua_tostring)
#define lua_tolstring ODBK_GET_FUN(__lua_tolstring)
#define lua_toboolean ODBK_GET_FUN(__lua_toboolean)
#define lua_isinteger ODBK_GET_FUN(__lua_isinteger)
#define lua_tointeger ODBK_GET_FUN_COMPACT(__lua_tointeger)
#define lua_tointegerx ODBK_GET_FUN_THUNK(__lua_tointegerx)
#define lua_tocfunction ODBK_GET_FUN(__lua_tocfunction)
#define lua_tonumber ODBK_GET_FUN(__lua_tonumber)
#define lua_tonumberx ODBK_GET_FUN(__lua_tonumberx)
#define lua_touserdata ODBK_GET_FUN(__lua_touserdata)
#define lua_load_510 ODBK_GET_FUN(__lua_load_510)
#define lua_load ODBK_GET_FUN(__lua_load)
#define lua_loadx ODBK_GET_FUN(__lua_loadx)
#define lua_call ODBK_GET_FUN(__lua_call)
#define lua_callk ODBK_GET_FUN(__lua_callk)
#define lua_pcall ODBK_GET_FUN(__lua_pcall)
#define lua_pcallk ODBK_GET_FUN(__lua_pcallk)
#define lua_newtable ODBK_GET_FUN(__lua_newtable)
#define lua_createtable ODBK_GET_FUN(__lua_createtable)
#define lua_next ODBK_GET_FUN((__lua_next)
#define lua_rawequal ODBK_GET_FUN(__lua_rawequal)
#define lua_getmetatable ODBK_GET_FUN(__lua_getmetatable)
#define lua_setmetatable ODBK_GET_FUN(__lua_setmetatable)
#define luaL_ref ODBK_GET_FUN(__luaL_ref)
#define luaL_unref ODBK_GET_FUN(__luaL_unref)
#define luaL_newmetatable ODBK_GET_FUN(__luaL_newmetatable)
#define luaL_loadbuffer ODBK_GET_FUN(__luaL_loadbuffer)
#define luaL_loadbufferx ODBK_GET_FUN(__luaL_loadbufferx)
#define luaL_loadfile ODBK_GET_FUN(__luaL_loadfile)
#define luaL_loadfilex ODBK_GET_FUN(__luaL_loadfilex)
#define luaL_register ODBK_GET_FUN(__luaL_register) // 5.1
#define luaL_openlibs ODBK_GET_FUN(__luaL_openlibs)
#define luaI_openlib ODBK_GET_FUN(__luaI_openlib)   // 5.1
#define lua_getupvalue ODBK_GET_FUN(__lua_getupvalue)
#define lua_setupvalue ODBK_GET_FUN(__lua_setupvalue)
#define lua_getfenv ODBK_GET_FUN(__lua_getfenv)
#define lua_setfenv ODBK_GET_FUN(__lua_setfenv)
#define lua_pushlightuserdata ODBK_GET_FUN(__lua_pushlightuserdata)
#define lua_cpcall ODBK_GET_FUN(__lua_cpcall)
#define lua_pushthread ODBK_GET_FUN(__lua_pushthread)
#define lua_newuserdata ODBK_GET_FUN(__lua_newuserdata)
#define luaL_newstate ODBK_GET_FUN(__luaL_newstate)
#define lua_checkstack ODBK_GET_FUN(__lua_checkstack)
#define lua_concat ODBK_GET_FUN(__lua_concat)
#define luaL_where ODBK_GET_FUN(__luaL_where)

// typedefs ALL lua APIs required by debugger.
typedef lua_State *(*lua_open_func_t)();
typedef lua_State *(*lua_newstate_func_t)(lua_Alloc, void *);
typedef void (*lua_close_func_t)(lua_State *);
typedef lua_State *(*lua_newthread_func_t)(lua_State *);
typedef int (*luaopen_base_func_t)(lua_State *L);
typedef void (*luaL_register_func_t)(lua_State *L, const char *libname,
                                     const luaL_Reg *l); // 5.1 only
typedef void (*luaI_openlib_func_t)(lua_State *L, const char *libname, const luaL_Reg *l,
                                    int nup); // 5.1 only
typedef void (*luaL_setfuncs_func_t)(lua_State *L, const luaL_Reg *l, int nup); // 5.2+
typedef int (*lua_error_func_t)(lua_State *);
typedef int (*lua_absindex_func_t)(lua_State *, int);
typedef int (*lua_sethook_func_t)(lua_State *, lua_Hook, int, int);
typedef int (*lua_gethookmask_func_t)(lua_State *);
typedef int (*lua_getinfo_func_t)(lua_State *, const char *, lua_Debug *ar);
typedef void (*lua_remove_func_t)(lua_State *, int);
typedef void (*lua_rotate_func_t)(lua_State *, int, int);
typedef void (*lua_settable_func_t)(lua_State *, int);
typedef void (*lua_gettable_func_t)(lua_State *, int);
typedef void (*lua_setglobal_func_t)(lua_State *, const char *);
typedef void (*lua_getglobal_func_t)(lua_State *, const char *);
typedef void (*lua_rawget_func_t)(lua_State *L, int idx);
typedef void (*lua_rawgeti_func_t)(lua_State *L, int idx, lua_Integer n);
typedef void (*lua_rawset_func_t)(lua_State *L, int idx);
typedef void (*lua_rawseti_func_t)(lua_State *L, int idx, lua_Integer n);
typedef void (*lua_pushstring_func_t)(lua_State *, const char *);
typedef void (*lua_pushlstring_func_t)(lua_State *, const char *, size_t);
typedef int (*lua_type_func_t)(lua_State *, int);
typedef const char *(*lua_typename_func_t)(lua_State *, int);
typedef void (*lua_settop_func_t)(lua_State *, int);
typedef const char *(*lua_getlocal_func_t)(lua_State *, const lua_Debug *, int);
typedef const char *(*lua_setlocal_func_t)(lua_State *, const lua_Debug *, int);
typedef int (*lua_getstack_func_t)(lua_State *, int, lua_Debug *);
typedef void (*lua_insert_func_t)(lua_State *, int);
typedef void (*lua_pushnil_func_t)(lua_State *);
typedef void (*lua_pushcclosure_func_t)(lua_State *, lua_CFunction, int);
typedef void (*lua_pushvalue_func_t)(lua_State *, int);
typedef void (*lua_pushinteger_func_t)(lua_State *, lua_Integer);
typedef void (*lua_pushnumber_func_t)(lua_State *, lua_Number);
typedef void (*lua_pushboolean_func_t)(lua_State *, int);
typedef const char *(*lua_tostring_func_t)(lua_State *, int);
typedef const char *(*lua_tolstring_func_t)(lua_State *, int, size_t *);
typedef int (*lua_toboolean_func_t)(lua_State *, int);
typedef int (*lua_isinteger_func_t)(lua_State *, int); // 5.3+
typedef lua_Integer (*lua_tointeger_func_t)(lua_State *, int);
typedef lua_Integer (*lua_tointegerx_func_t)(lua_State *, int,
                                             int *); // lua_tointeger just a macro in 5.2+
typedef lua_CFunction (*lua_tocfunction_func_t)(lua_State *, int);
typedef lua_Number (*lua_tonumber_func_t)(lua_State *, int);
typedef lua_Number (*lua_tonumberx_func_t)(lua_State *, int, int *);
typedef void *(*lua_touserdata_func_t)(lua_State *, int);
typedef int (*lua_gettop_func_t)(lua_State *);
typedef int (*lua_load_510_func_t)(lua_State *, lua_Reader, void *, const char *chunkname);
typedef int (*lua_load_func_t)(lua_State *, lua_Reader, void *, const char *chunkname, ...
                               /*const char *mode*/);
typedef void (*lua_call_func_t)(lua_State *, int, int);
typedef void (*lua_callk_func_t)(lua_State *, int, int, int, lua_CFunction);
typedef int (*lua_pcall_func_t)(lua_State *, int, int, int);
typedef int (*lua_pcallk_func_t)(lua_State *, int, int, int, ptrdiff_t, lua_CFunction); // 5.3+
 typedef int (*lua_pcallk_52_func_t)(lua_State *, int, int, int, int, lua_CFunction); // 5.2-
typedef void (*lua_newtable_func_t)(lua_State *);
typedef void (*lua_createtable_func_t)(lua_State *, int, int);
typedef int (*lua_next_func_t)(lua_State *, int);
typedef int (*lua_rawequal_func_t)(lua_State *L, int idx1, int idx2);
typedef int (*lua_getmetatable_func_t)(lua_State *, int objindex);
typedef int (*lua_setmetatable_func_t)(lua_State *, int objindex);
typedef int (*luaL_ref_func_t)(lua_State *L, int t);
typedef void (*luaL_unref_func_t)(lua_State *L, int t, int ref);
typedef int (*luaL_newmetatable_func_t)(lua_State *L, const char *tname);
typedef int (*luaL_loadbuffer_func_t)(lua_State *L, const char *buff, size_t sz, const char *name);
typedef int (*luaL_loadbufferx_func_t)(lua_State *L, const char *buff, size_t sz, const char *name,
                                       const char *mode);
typedef int (*luaL_loadfile_func_t)(lua_State *L, const char *fileName);
typedef int (*luaL_loadfilex_func_t)(lua_State *L, const char *fileName, const char *mode);
typedef void (*luaL_openlibs_func_t)(lua_State *L);
typedef const char *(*lua_getupvalue_func_t)(lua_State *L, int funcindex, int n);
typedef const char *(*lua_setupvalue_func_t)(lua_State *L, int funcindex, int n);
typedef void (*lua_getfenv_func_t)(lua_State *L, int index);
typedef int (*lua_setfenv_func_t)(lua_State *L, int index);
typedef void (*lua_pushlightuserdata_func_t)(lua_State *L, void *p);
typedef int (*lua_cpcall_func_t)(lua_State *L, lua_CFunction func, void *ud);
typedef int (*lua_pushthread_func_t)(lua_State *L);
typedef void *(*lua_newuserdata_func_t)(lua_State *L, size_t size);
typedef lua_State *(*luaL_newstate_func_t)();
typedef int (*lua_checkstack_func_t)(lua_State *L, int extra);
typedef int (*lua_concat_func_t)(lua_State *L, int n);
typedef int (*luaL_where_func_t)(lua_State *L, int level);

///////////// ALL Lua APIs required by debugger function pointer ////////////////

extern lua_open_func_t __lua_open;
extern lua_newstate_func_t __lua_newstate;
extern luaopen_base_func_t __luaopen_base;
extern lua_close_func_t __lua_close;
extern lua_newthread_func_t __lua_newthread;
extern lua_error_func_t __lua_error;
extern lua_absindex_func_t __lua_absindex;
extern lua_gettop_func_t __lua_gettop;
extern lua_sethook_func_t __lua_sethook;
extern lua_gethookmask_func_t __lua_gethookmask;
extern lua_getinfo_func_t __lua_getinfo;
extern lua_remove_func_t __lua_remove;
extern lua_rotate_func_t __lua_rotate;
extern lua_settable_func_t __lua_settable;
extern lua_gettable_func_t __lua_gettable;
extern lua_setglobal_func_t __lua_setglobal;
extern lua_getglobal_func_t __lua_getglobal;
extern lua_rawget_func_t __lua_rawget;
extern lua_rawgeti_func_t __lua_rawgeti;
extern lua_rawset_func_t __lua_rawset;
extern lua_rawseti_func_t __lua_rawseti;
extern lua_pushstring_func_t __lua_pushstring;
extern lua_pushlstring_func_t __lua_pushlstring;
extern lua_type_func_t __lua_type;
extern lua_typename_func_t __lua_typename;
extern lua_settop_func_t __lua_settop;
extern lua_getlocal_func_t __lua_getlocal;
extern lua_setlocal_func_t __lua_setlocal;
extern lua_getstack_func_t __lua_getstack;
extern lua_insert_func_t __lua_insert;
extern lua_pushnil_func_t __lua_pushnil;
extern lua_pushvalue_func_t __lua_pushvalue;
extern lua_pushinteger_func_t __lua_pushinteger;
extern lua_pushnumber_func_t __lua_pushnumber;
extern lua_pushboolean_func_t __lua_pushboolean;
extern lua_pushcclosure_func_t __lua_pushcclosure;
extern lua_tostring_func_t __lua_tostring;
extern lua_tolstring_func_t __lua_tolstring;
extern lua_toboolean_func_t __lua_toboolean;
extern lua_isinteger_func_t __lua_isinteger;
extern lua_tointeger_func_t __lua_tointeger;
extern lua_tointegerx_func_t __lua_tointegerx;
extern lua_tocfunction_func_t __lua_tocfunction;
extern lua_tonumber_func_t __lua_tonumber;
extern lua_tonumberx_func_t __lua_tonumberx;
extern lua_touserdata_func_t __lua_touserdata;
extern lua_load_func_t __lua_load;
extern lua_load_func_t __lua_loadx; // luajit
extern lua_call_func_t __lua_call;
extern lua_callk_func_t __lua_callk;
extern lua_pcall_func_t __lua_pcall;
extern lua_pcallk_func_t __lua_pcallk;
extern lua_newtable_func_t __lua_newtable;
extern lua_createtable_func_t __lua_createtable;
extern lua_next_func_t __lua_next;
extern lua_rawequal_func_t __lua_rawequal;
extern lua_getmetatable_func_t __lua_getmetatable;
extern lua_setmetatable_func_t __lua_setmetatable;
extern luaL_ref_func_t __luaL_ref;
extern luaL_unref_func_t __luaL_unref;
extern luaL_newmetatable_func_t __luaL_newmetatable;
extern luaL_loadbuffer_func_t __luaL_loadbuffer;
extern luaL_loadbufferx_func_t __luaL_loadbufferx;
extern luaL_loadfile_func_t __luaL_loadfile;
extern luaL_loadfilex_func_t __luaL_loadfilex;
extern luaL_register_func_t __luaL_register; // 5.1
extern luaL_openlibs_func_t __luaL_openlibs;
extern luaI_openlib_func_t __luaI_openlib;   // 5.1
extern lua_getupvalue_func_t __lua_getupvalue;
extern lua_setupvalue_func_t __lua_setupvalue;
extern lua_getfenv_func_t __lua_getfenv;
extern lua_setfenv_func_t __lua_setfenv;
extern lua_pushlightuserdata_func_t __lua_pushlightuserdata;
extern lua_cpcall_func_t __lua_cpcall;
extern lua_pushthread_func_t __lua_pushthread;
extern lua_newuserdata_func_t __lua_newuserdata;
extern luaL_newstate_func_t __luaL_newstate;
extern lua_checkstack_func_t __lua_checkstack;
extern lua_concat_func_t __lua_concat;
extern luaL_where_func_t __luaL_where;

// ---------------------- compact functions for ----------------------------
extern lua_Integer(__lua_tointeger__compact)(lua_State *, int);

// ------------- 32bits thunks for support lua-5.3.x -----------------------
#if ODBK_BITS == 32
extern void(__lua_rawgeti__thunk)(lua_State *L, int idx, lua_Integer n);
extern void(__lua_rawseti__thunk)(lua_State *L, int idx, lua_Integer n);
extern void(__lua_pushinteger__thunk)(lua_State *, lua_Integer);
extern lua_Integer(__lua_tointegerx__thunk)(lua_State *, int, int *);
#endif

/*
start,attach
*/
bool odbk_load_lua(void *hLua);

// try load lua, if succed, then the debug backend can init succed, otherwise, can't debug target.
bool odbk_load_lua_internal(odbk_core *ctx, void *hLua = nullptr);

int odbk_lua_version();
void odbk_lua_unload(); // TODO:
}
