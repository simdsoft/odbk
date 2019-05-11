#pragma once

#include <windows.h>
#include <dbgeng.h>
#include <dbghelp.h>
#include <vector>

typedef struct _UNICODE_STRING
{ // UNICODE_STRING structure
  USHORT Length;
  USHORT MaximumLength;
  PWSTR Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

typedef BOOL(WINAPI *PFNSYMENUMSYMBOLS)(__in HANDLE, __in ULONG64, __in_opt PCSTR,
                                       __in PSYM_ENUMERATESYMBOLS_CALLBACK, __in_opt PVOID);
typedef BOOL(WINAPI *PFNSYMINITIALIZE)(__in HANDLE, __in_opt PCSTR, __in BOOL);
typedef BOOL(WINAPI *PFNSYMCLEANUP)(__in HANDLE);
typedef DWORD64(WINAPI *PFNSYMLOADMODULE64)(__in HANDLE, __in_opt HANDLE, __in_opt PCSTR,
                                           __in_opt PCSTR, __in DWORD64, __in DWORD);
typedef DWORD64(WINAPI *PFNSYMUNLOADMODULE64)(__in HANDLE, __in DWORD64);
typedef BOOL(WINAPI *PFNSYMGETMODULEINFO64)(HANDLE, DWORD64 dwAddr,
                                              PIMAGEHLP_MODULE64 ModuleInfo);
typedef BOOL(WINAPI *PFNSYMGETLINEFROMADDR64)(_In_ HANDLE hProcess, _In_ DWORD64 dwAddr,
                                              _Out_ PDWORD pdwDisplacement,
                                              _Out_ PIMAGEHLP_LINE64 Line);
typedef BOOL(WINAPI *PFNSTACKWALK64)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID,
                                    PREAD_PROCESS_MEMORY_ROUTINE64,
                                    PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64,
                                    PTRANSLATE_ADDRESS_ROUTINE64);
typedef PVOID(WINAPI *PFNSYMFUNCTIONTABLEACCESS64)(HANDLE, DWORD64);
typedef BOOL(WINAPI *PFNSYMGETSYMFROMADDR64)(HANDLE, DWORD64, PDWORD64, PIMAGEHLP_SYMBOL64);
typedef DWORD64(WINAPI *PFNSYMGETMODULEBASE64)(HANDLE, DWORD64);

typedef VOID(WINAPI *PFNRTLCAPTURECONTEXT)(PCONTEXT);
typedef USHORT(WINAPI *PFNRTLCAPTURESTACKBACKTRACE)(ULONG, ULONG, PVOID *, PULONG);
typedef PVOID(WINAPI *PFNIMAGERVATOVA)(PIMAGE_NT_HEADERS, PVOID, ULONG, PIMAGE_SECTION_HEADER *);

typedef ULONG(WINAPI *PFNLDRLOCKLOADERLOCK)(ULONG flags, PULONG disposition, PULONG cookie);
typedef LONG(WINAPI *PFNLDRUNLOCKLOADERLOCK)(ULONG flags, ULONG cookie);
typedef decltype(&OutputDebugStringA) PFNOUTPUTDEBUGSTRINGA;
typedef decltype(&OutputDebugStringW) PFNOUTPUTDEBUGSTRINGW;
typedef NTSTATUS(WINAPI *PFNLDRLOADDLL)(IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL,
                                       IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle);


namespace odbk
{
extern PFNSYMENUMSYMBOLS SymEnumSymbols;
extern PFNSYMINITIALIZE SymInitialize;
extern PFNSYMCLEANUP SymCleanup;
extern PFNSYMLOADMODULE64 SymLoadModule64;
extern PFNSYMUNLOADMODULE64 SymUnloadModule64;
extern PFNSYMGETMODULEINFO64 SymGetModuleInfo64;
extern PFNSYMGETLINEFROMADDR64 SymGetLineFromAddr64;
extern PFNSTACKWALK64 StackWalk64;
extern PFNSYMFUNCTIONTABLEACCESS64 SymFunctionTableAccess64;
extern PFNSYMGETSYMFROMADDR64 SymGetSymFromAddr64;
extern PFNSYMGETMODULEBASE64 SymGetModuleBase64;

extern PFNRTLCAPTURECONTEXT RtlCaptureContext;
extern PFNRTLCAPTURESTACKBACKTRACE RtlCaptureStackBackTrace;
extern PFNIMAGERVATOVA ImageRvaToVa;

extern PFNOUTPUTDEBUGSTRINGW OutputDebugStringW; // capture debug log.
extern PFNOUTPUTDEBUGSTRINGA OutputDebugStringA;  // capture debug log.
extern PFNLDRLOADDLL LdrLoadDll;                 // Low level API at ntdll.dll
extern PFNLDRLOCKLOADERLOCK LdrLockLoaderLock;
extern PFNLDRUNLOCKLOADERLOCK LdrUnlockLoaderLock;

/**
 * Dynamically loads the Debug Help library functions from the dbghelp.dll in
 * the same directory as the specified executable.
 */
bool load_dbghelp(const char *dbghelpPath);

bool locate_sym_file(const IMAGEHLP_MODULE64 &moduleInfo, char fileName[_MAX_PATH]);

} // namespace odbk
