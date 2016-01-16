// Copyright (2015) Gustav

#include "finans/core/os.h"

#ifdef FINANS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <io.h>
#endif

const char FOLDER_SEPERATOR =
#if defined(FINANS_UNIX)
'/';
#elif defined(FINANS_WINDOWS)
'\\';
#else
'0';
#error "unsupported os"
#endif


std::string FindUserPath() {
  // http://stackoverflow.com/questions/2552416/how-can-i-find-the-users-home-dir-in-a-cross-platform-manner-using-c
#ifdef FINANS_WINDOWS
  TCHAR path[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, path)))
  {
    TCHAR oemPath[MAX_PATH] = { 0, };
    CharToOem(path, oemPath);
    const auto path = EndWithSlash(oemPath) + "finans\\";
    if (!CreateDirectory(path.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
      return "";
    }
    else {
      return path;
    }
  }
#else
#error "IMPLEMENT ME"
#endif
  return "";
}

std::string EndWithSlash(const std::string& path) {
  if (path.empty()) return "";
  if (path.length() - 1 == FOLDER_SEPERATOR) return path;
  else return path + FOLDER_SEPERATOR;
}
