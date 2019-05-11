#include <Windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include "odbk_dbghelp.h"
#include "odbk_utils.h"

// Macro for convenient pointer addition.
// Essentially treats the last two parameters as DWORDs.  The first
// parameter is used to typecast the result to the appropriate pointer type.
#define MAKE_PTR(cast, ptr, addValue) (cast)((DWORD)(ptr) + (DWORD)(addValue))

void odbk_get_exports(HMODULE hModule, std::unordered_map<std::string, uintptr_t> &exports)
{
  int i                                         = 0;
  PIMAGE_DOS_HEADER pImageDosHeader             = NULL;
  PIMAGE_NT_HEADERS pImageNtHeader              = NULL;
  PIMAGE_EXPORT_DIRECTORY pImageExportDirectory = NULL;

  pImageDosHeader = (PIMAGE_DOS_HEADER)hModule;
  pImageNtHeader  = (PIMAGE_NT_HEADERS)((INT_PTR)hModule + (INT_PTR)pImageDosHeader->e_lfanew);
  pImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(
      (uintptr_t)hModule +
      pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

  DWORD dwExportRVA =
      pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
  DWORD dwExportSize =
      pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

  DWORD *pAddressOfFunction =
      (DWORD *)(pImageExportDirectory->AddressOfFunctions + (uintptr_t)hModule);
  DWORD *pAddressOfNames = (DWORD *)(pImageExportDirectory->AddressOfNames + (uintptr_t)hModule);
  DWORD dwNumberOfNames  = (DWORD)(pImageExportDirectory->NumberOfNames);
  DWORD dwBase           = (DWORD)(pImageExportDirectory->Base);

  WORD *pAddressOfNameOrdinals =
      (WORD *)(pImageExportDirectory->AddressOfNameOrdinals + (uintptr_t)hModule);

  for (i = 0; i < (int)dwNumberOfNames; i++)
  {
    char *strFunction = (char *)(pAddressOfNames[i] + (DWORD_PTR)hModule);
    exports.emplace(strFunction, (uintptr_t)(pAddressOfFunction[pAddressOfNameOrdinals[i]] +
                                             (uintptr_t)hModule));
  }
}

static PIMAGE_NT_HEADERS PEHeaderFromHModule(HMODULE hModule)
{
  PIMAGE_NT_HEADERS pNTHeader = 0;

  __try
  {
    if (PIMAGE_DOS_HEADER(hModule)->e_magic != IMAGE_DOS_SIGNATURE)
      __leave;

    pNTHeader = PIMAGE_NT_HEADERS(PBYTE(hModule) + PIMAGE_DOS_HEADER(hModule)->e_lfanew);

    if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
      pNTHeader = 0;
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {}

  return pNTHeader;
}
bool odbk_get_imports(HMODULE hModule, std::vector<std::string> &imports)
{
  PIMAGE_NT_HEADERS pExeNTHdr = PEHeaderFromHModule(hModule);

  if (!pExeNTHdr)
  {
    return false;
  }

  DWORD importRVA =
      pExeNTHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
  if (!importRVA)
  {
    return false;
  }

  // Convert imports RVA to a usable pointer
  PIMAGE_IMPORT_DESCRIPTOR pImportDesc = MAKE_PTR(PIMAGE_IMPORT_DESCRIPTOR, hModule, importRVA);

  // Iterate through each import descriptor, and redirect if appropriate
  while (pImportDesc->FirstThunk)
  {
    PSTR pszImportModuleName = MAKE_PTR(PSTR, hModule, pImportDesc->Name);
    imports.push_back(pszImportModuleName);
    pImportDesc++; // Advance to next import descriptor
  }

  return true;
}

std::string odbk_sfmt(const char *format, ...)
{
#define YASIO_VSNPRINTF_BUFFER_LENGTH 256
  va_list args;
  std::string buffer(YASIO_VSNPRINTF_BUFFER_LENGTH, '\0');

  va_start(args, format);
  int nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
  va_end(args);

  if (nret >= 0)
  {
    if ((unsigned int)nret < buffer.length())
    {
      buffer.resize(nret);
    }
    else if ((unsigned int)nret > buffer.length())
    { // VS2015/2017 or later Visual Studio Version
      buffer.resize(nret);

      va_start(args, format);
      nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
      va_end(args);
    }
    // else equals, do nothing.
  }
  else
  { // less or equal VS2013 and Unix System glibc implement.
    do
    {
      buffer.resize(buffer.length() * 3 / 2);

      va_start(args, format);
      nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
      va_end(args);

    } while (nret < 0);

    buffer.resize(nret);
  }

  return buffer;
}

int odbk_replace_extension(char fileName[_MAX_PATH], int n, const char *ext, int size)
{
  int dot = n - 1;
  for (; fileName[dot] != '.';)
    --dot;
  memcpy(&fileName[dot] + 1, ext, size);
  return dot + size;
}

bool odbk_file_exists(const char* path) {
  WIN32_FILE_ATTRIBUTE_DATA attrs = {0};
  return GetFileAttributesExA(path, GetFileExInfoStandard, &attrs) &&
         !(attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

void odbk_get_file_title(const char *fileName, char fileTitle[_MAX_PATH])
{

  const char *slash1 = strrchr(fileName, '\\');
  const char *slash2 = strrchr(fileName, '/');

  const char *pathEnd = max(slash1, slash2);

  if (pathEnd == NULL)
  {
    // There's no path so the whole thing is the file title.
    strcpy(fileTitle, fileName);
  }
  else
  {
    strcpy(fileTitle, pathEnd + 1);
  }
}

void odbk_get_file_path(const char *fileName, char path[_MAX_PATH])
{

  const char *slash1 = strrchr(fileName, '\\');
  const char *slash2 = strrchr(fileName, '/');

  const char *pathEnd = max(slash1, slash2);

  if (pathEnd == NULL)
  {
    // There's no path on the file name.
    path[0] = 0;
  }
  else
  {
    size_t length = pathEnd - fileName + 1;
    memcpy(path, fileName, length);
    path[length] = 0;
  }
}


