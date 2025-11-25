#pragma once

#include <wrl.h>        // CommandLineToArgvW
#include <Shlobj.h>		// for SHGetKnownFolderPath()
#include <strsafe.h>
#include "WebView2.h"
#include <windows.h>
#include <filesystem>
#include "resource.h"

Microsoft::WRL::ComPtr<ICoreWebView2> webViewWindow;
Microsoft::WRL::ComPtr<ICoreWebView2Controller> webViewController;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

std::wstring LoadStringAsWstr(HINSTANCE hInstance, UINT uID);
std::wstring LoadStringFromResource(HINSTANCE instance, UINT stringID);
std::wstring LoadHtml();


std::wstring ConvertUtf8ToWide(const std::string& str);
std::string ConvertWideToUtf8(const std::wstring& wstr);