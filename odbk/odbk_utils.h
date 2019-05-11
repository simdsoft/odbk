#pragma once
#include <unordered_set>
#include <unordered_map>
#include <string>

void odbk_get_exports(HMODULE hModule, std::unordered_map<std::string, uintptr_t> & exports);

bool odbk_get_imports(HMODULE hModule, std::vector<std::string> &imports);

/*--- This is a C++ universal sprintf in the future.
 **  @pitfall: The behavior of vsnprintf between VS2013 and VS2015/2017 is
 *different
 **      VS2013 or Unix-Like System will return -1 when buffer not enough, but
 *VS2015/2017 will return the actural needed length for buffer at this station
 **      The _vsnprintf behavior is compatible API which always return -1 when
 *buffer isn't enough at VS2013/2015/2017
 **      Yes, The vsnprintf is more efficient implemented by MSVC 19.0 or later,
 *AND it's also standard-compliant, see reference:
 *http://www.cplusplus.com/reference/cstdio/vsnprintf/
 */
std::string odbk_sfmt(const char *format, ...);

int odbk_replace_extension(char fileName[_MAX_PATH], int n, const char *ext, int size);

template <int _Size>
inline int odbk_replace_extension(char fileName[_MAX_PATH], int n, const char (&ext)[_Size])
{
  return odbk_replace_extension(fileName, n, ext, _Size);
}


bool odbk_file_exists(const char *path);
void odbk_get_file_path(const char *fileName, char path[_MAX_PATH]);
void odbk_get_file_title(const char *fileName, char fileTitle[_MAX_PATH]);
