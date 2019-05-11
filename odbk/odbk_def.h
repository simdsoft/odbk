#pragma once

#if defined(_WIN32)
#  if defined(_WINDLL)
#    if defined(ODBK_EXPORTS)
#      define ODBK_API __declspec(dllexport)
#else
#      define ODBK_API __declspec(dllimport)
#endif
#else
#define ODBK_API
#endif
#else
#  define ODBK_API
#endif

ODBK_API void odbk_init();

/*********************************/
/** 64bits Program Sense Macros **/
/*********************************/
#  if defined(_M_X64) || defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(__x86_64)
#    define ODBK_BITS 64
#  else
#    define ODBK_BITS 32
#  endif

#  define ODBK_GET_FUN(x) x
#  if ODBK_BITS == 32
#    define ODBK_GET_FUN_THUNK(x) x##__thunk
#  else
#    define ODBK_GET_FUN_THUNK ODBK_GET_FUN
#  endif

#  define ODBK_GET_FUN_COMPACT(x) x##__compact

#  define ODBK_GET_PROC_ADDR(h, x) x = reinterpret_cast<decltype(x)>(::GetProcAddress(h, #  x))

#  define ODBK_LOGINFO(format, ...)                                                                \
    do                                                                                             \
    {                                                                                              \
      auto content = odbk_sfmt((format "\r\n"), ##__VA_ARGS__);                                    \
      singleton<odbk_core>::instance()->print(content);                                            \
    } while (false)

#  define ODBK_LOGWARN(format, ...)                                                                \
    do                                                                                             \
    {                                                                                              \
      auto content = odbk_sfmt((format "\r\n"), ##__VA_ARGS__);                                    \
      singleton<odbk_core>::instance()->print(content);                                            \
    } while (false)

#  define ODBK_LOGERR(format, ...)                                                                 \
    do                                                                                             \
    {                                                                                              \
      auto content = odbk_sfmt((format "\r\n"), ##__VA_ARGS__);                                    \
      singleton<odbk_core>::instance()->print(content);                                            \
    } while (false)

class odbk_core;
