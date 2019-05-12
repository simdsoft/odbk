#include "odbk_hook.h"
#include "odbk_lload.h"
#include "odbk_core.h"
#include "odbk_lhook.h"

lua_State *lua_open_hook() { return lua_open(); }
lua_State *lua_newstate_hook(lua_Alloc a, void *p) { return lua_newstate(a, p); }
lua_State *luaL_newstate_hook() { return luaL_newstate(); }
int luaopen_base_hook(lua_State *L) { return luaopen_base(L); }
void lua_close_hook(lua_State *L) { lua_close(L); }

lua_State *lua_newthread_hook(lua_State *L) { return lua_newthread(L); }
int lua_pcall_hook(lua_State *L, int nargs, int nresults, int errfunc)
{
  return lua_pcall(L, nargs, nresults, errfunc);
}
int lua_pcallk_hook(lua_State *L, int nargs, int nresults, int errfunc, int ctx, lua_CFunction k)
{
  return lua_pcallk(L, nargs, nresults, errfunc, ctx, k);
}

void lua_call_hook(lua_State *L, int nargs, int nresults) { lua_call(L, nargs, nresults); }

void lua_callk_hook(lua_State *L, int nargs, int nresults, int ctk, lua_CFunction k)
{
  lua_callk(L, nargs, nresults, ctk, k);
}

int luaL_newmetatable_hook(lua_State *L, const char *tname)
{ // hook this for class name register.
  int ret = luaL_newmetatable(L, tname);
  return ret;
}

int lua_sethook_hook(lua_State *L, lua_Hook f, int mask, int count)
{
  auto ret = lua_sethook(L, f, mask, count);
  return ret;
}

int luaL_loadbufferx_hook(lua_State *L, const char *buff, size_t sz, const char *name,
                          const char *mode)
{
  // ptifall: For static lua engine library, we needs hook luaL_loadbuffer/luaL_loadbufferx
  // Actually, luaL_loadbuffer will call luaL_loadbufferx, but static library only target exe used
  // function can be retrive address from .pdb
  int ret = -1;
  if (luaL_loadbufferx)
    ret = luaL_loadbufferx(L, buff, sz, name, mode);
  else if (luaL_loadbuffer)
    ret = luaL_loadbuffer(L, buff, sz, name);
  return ret;
}

int luaL_loadbuffer_hook(lua_State *L, const char *buff, size_t sz, const char *name)
{
  return luaL_loadbufferx_hook(L, buff, sz, name, NULL);
}

int luaL_loadfilex_hook(lua_State *L, const char *fileName, const char *mode)
{
  int ret = -1;
  if (luaL_loadfilex)
    ret = luaL_loadfilex(L, fileName, mode);
  else if (luaL_loadfile)
    ret = luaL_loadfile(L, fileName);
  return ret;
}
int luaL_loadfile_hook(lua_State *L, const char *fileName)
{
  return luaL_loadfilex_hook(L, fileName, NULL);
}

int lua_load_hook(lua_State *L, lua_Reader reader, void *data, const char *name, const char *mode)
{
  // c calling convention, may be no need to call, pass more argument than original prototype is ok.
  // consider does x64 call convention work well?
  int ret          = -1;
  auto lua_version = odbk_lua_version();
  if (lua_version <= 510)
    ret = lua_load(L, reader, data, name);
  else
    ret = lua_load(L, reader, data, name, mode);
  return ret;
}

int lua_load_510_hook(lua_State *L, lua_Reader reader, void *data, const char *name)
{
  return lua_load_hook(L, reader, data, name, NULL);
}

void luaL_openlibs_hook(lua_State *L) { luaL_openlibs(L); }

void luaI_openlib_hook(lua_State *L, const char *libname, const luaL_Reg *l, int nup)
{
  luaI_openlib(L, libname, l, nup);
}

void odbk_install_lua_hooks()
{
#define __HOOKFN(f) odbk_hook(&(PVOID &)f, f##_hook)
#define __HOOKFN_ALIAS(f, alias) odbk_hook(&(PVOID &)f, alias##_hook)
  auto lua_version = odbk_lua_version();

  __HOOKFN(lua_open); // lua 5.0.x only
  __HOOKFN(lua_newstate);
  __HOOKFN(luaL_newstate);
  __HOOKFN(luaopen_base);
  __HOOKFN(lua_close);
  __HOOKFN(lua_newthread);
  __HOOKFN(lua_pcall);
  __HOOKFN(lua_pcallk);
  __HOOKFN(lua_call);
  __HOOKFN(lua_callk);

  __HOOKFN(luaL_newmetatable);
  __HOOKFN(lua_sethook);

  __HOOKFN(luaL_loadbufferx); // Lua 5.2.x
  __HOOKFN(luaL_loadbuffer);  // Lua 5.1.x

  __HOOKFN(luaL_loadfilex);
  __HOOKFN(luaL_loadfile);

  if (lua_version > 510)
  {
    __HOOKFN(lua_load); // 5.2 5.3: lua_load, luajit: lua_loadx
  }
  else
  {
    __HOOKFN_ALIAS(lua_load, lua_load_510); // lua5.1 load no parameter:mode
  }

  __HOOKFN(luaL_newstate);

  __HOOKFN(luaL_openlibs);

  if (lua_version < 530)
  {
    __HOOKFN(luaI_openlib);
  }

  odbk_enable_hook();
  OutputDebugStringA("odbk_install_lua_hooks\n");
}
