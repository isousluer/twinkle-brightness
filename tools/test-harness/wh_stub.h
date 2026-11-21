#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <map>
std::map<std::wstring, std::wstring>& StubSettings();
inline void Wh_Log_(PCWSTR fmt, ...) { va_list a; va_start(a, fmt); vfwprintf(stdout, fmt, a); va_end(a); fwprintf(stdout, L"\n"); fflush(stdout); }
#define Wh_Log(fmt, ...) Wh_Log_(fmt, ##__VA_ARGS__)
inline PCWSTR Wh_GetStringSetting(PCWSTR name, ...) { auto& m = StubSettings(); auto it = m.find(name); return _wcsdup(it != m.end() ? it->second.c_str() : L""); }
inline void Wh_FreeStringSetting(PCWSTR s) { free((void*)s); }
inline int Wh_GetIntSetting(PCWSTR name, ...) { auto& m = StubSettings(); auto it = m.find(name); return it != m.end() ? _wtoi(it->second.c_str()) : 0; }
inline BOOL Wh_SetFunctionHook(void*, void*, void**) { return TRUE; }
