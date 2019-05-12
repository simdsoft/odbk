#pragma once

#if defined(_WIN32)
#  if defined(_WINDLL)
#    if defined(odbk_EXPORTS)
#      define ODBK_API __declspec(dllexport)
#    else
#      define ODBK_API __declspec(dllimport)
#    endif
#  else
#    define ODBK_API
#  endif
#else
#  define ODBK_API
#endif

/*********************************/
/** 64bits Program Sense Macros **/
/*********************************/
#if defined(_M_X64) || defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(__x86_64)
#  define ODBK_BITS 64
#else
#  define ODBK_BITS 32
#endif
