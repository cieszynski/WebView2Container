#pragma once

#include <wrl.h>        // CommandLineToArgvW
#include <Shlobj.h>		// for SHGetKnownFolderPath()
#include <strsafe.h>
#include "WebView2.h"
#include <windows.h>
#include <filesystem>

Microsoft::WRL::ComPtr<ICoreWebView2> webViewWindow;
Microsoft::WRL::ComPtr<ICoreWebView2Controller> webViewController;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);