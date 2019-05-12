#include "odbk_dbghelp.h"
#include "odbk_internal.h"
#include "odbk_utils.h"

namespace odbk
{
PFNSYMENUMSYMBOLS SymEnumSymbols;
PFNSYMINITIALIZE SymInitialize;
PFNSYMCLEANUP SymCleanup;
PFNSYMLOADMODULE64 SymLoadModule64;
PFNSYMUNLOADMODULE64 SymUnloadModule64;
PFNSYMGETMODULEINFO64 SymGetModuleInfo64;
PFNSYMGETLINEFROMADDR64 SymGetLineFromAddr64;
PFNSTACKWALK64 StackWalk64;
PFNSYMFUNCTIONTABLEACCESS64 SymFunctionTableAccess64;
PFNSYMGETSYMFROMADDR64 SymGetSymFromAddr64;
PFNSYMGETMODULEBASE64 SymGetModuleBase64;

PFNRTLCAPTURECONTEXT RtlCaptureContext;
PFNRTLCAPTURESTACKBACKTRACE RtlCaptureStackBackTrace;
PFNIMAGERVATOVA ImageRvaToVa;

PFNOUTPUTDEBUGSTRINGW OutputDebugStringW; // capture debug log.
PFNOUTPUTDEBUGSTRINGA OutputDebugStringA; // capture debug log.
PFNLDRLOADDLL LdrLoadDll;                 // Low level API at ntdll.dll
PFNLDRLOCKLOADERLOCK LdrLockLoaderLock;
PFNLDRUNLOCKLOADERLOCK LdrUnlockLoaderLock;

bool load_dbghelp(const char *dbghelpPath)
{
#define __INIT_FN(x) ODBK_GET_PROC_ADDR(hModule, x)
  /// Get functions need from kernel32.dll
  HMODULE hModule = GetModuleHandleW(L"kernel32.dll");
  if (hModule == nullptr)
    return false;
  __INIT_FN(RtlCaptureContext);

  /// Get functions needed from ntdll.dll
  hModule = GetModuleHandleW(L"ntdll.dll");
  if (hModule == nullptr)
    return false;

  __INIT_FN(RtlCaptureStackBackTrace);
  __INIT_FN(OutputDebugStringW);
  __INIT_FN(OutputDebugStringA);
  __INIT_FN(LdrLoadDll);
  __INIT_FN(LdrLockLoaderLock);
  __INIT_FN(LdrUnlockLoaderLock);

  /// Get functions needed from dbghelp.dll, TODO: this module may be need store & FreeLibrary
  hModule = LoadLibraryA(dbghelpPath);

  if (hModule == nullptr)
    return false;

  __INIT_FN(SymInitialize);
  __INIT_FN(SymCleanup);
  __INIT_FN(SymEnumSymbols);
  __INIT_FN(SymLoadModule64);
  __INIT_FN(SymUnloadModule64);
  __INIT_FN(SymGetModuleInfo64);
  __INIT_FN(SymGetLineFromAddr64);
  __INIT_FN(StackWalk64);
  __INIT_FN(SymGetSymFromAddr64);
  __INIT_FN(SymFunctionTableAccess64);
  __INIT_FN(SymGetModuleBase64);
  __INIT_FN(ImageRvaToVa);

  return true;
}

bool locate_sym_file(const IMAGEHLP_MODULE64 &moduleInfo, char fileName[_MAX_PATH])
{

  // The search order for symbol files is described here:
  // http://msdn2.microsoft.com/en-us/library/ms680689.aspx

  // This function doesn't currently support the full spec.

  const char *imageFileName = moduleInfo.LoadedImageName;

  // First check the absolute path specified in the CodeView data.
  if (odbk_file_exists(moduleInfo.CVData))
  {
    strncpy(fileName, moduleInfo.CVData, _MAX_PATH);
    return true;
  }

  char symbolTitle[_MAX_PATH];
  odbk_get_file_title(moduleInfo.CVData, symbolTitle);

  // Now check in the same directory as the image.

  char imagePath[_MAX_PATH];
  odbk_get_file_path(imageFileName, imagePath);

  strcat(imagePath, symbolTitle);

  if (odbk_file_exists(imagePath))
  {
    strncpy(fileName, imagePath, _MAX_PATH);
    return true;
  }

  return false;
}

} // namespace odbk
