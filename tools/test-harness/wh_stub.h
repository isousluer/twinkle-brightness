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
// Mod storage: kept in memory, so the harness never touches Windhawk's.
std::map<std::wstring, std::wstring>& StubStorage();
inline BOOL Wh_SetStringValue(PCWSTR name, PCWSTR value) { StubStorage()[name] = value; return TRUE; }
inline size_t Wh_GetStringValue(PCWSTR name, PWSTR buf, size_t size) { auto& m = StubStorage(); auto it = m.find(name); std::wstring v = it != m.end() ? it->second : L""; if (size) { wcsncpy(buf, v.c_str(), size - 1); buf[size - 1] = 0; } return v.size(); }
inline BOOL Wh_SetIntValue(PCWSTR name, int value) { StubStorage()[name] = std::to_wstring(value); return TRUE; }
inline int Wh_GetIntValue(PCWSTR name, int def) { auto& m = StubStorage(); auto it = m.find(name); return it != m.end() ? _wtoi(it->second.c_str()) : def; }
inline BOOL Wh_DeleteValue(PCWSTR name) { StubStorage().erase(name); return TRUE; }
