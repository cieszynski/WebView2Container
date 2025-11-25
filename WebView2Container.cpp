#ifndef UNICODE
#define UNICODE
#endif 

#include "WebView2Container.h"

//static int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)

{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		MessageBox(
			nullptr,
			L"Call to CreateWindow failed!",
			L"WebView2Runner",
			MB_ICONERROR);
		return 0;
	}
	else
	{
		PWSTR localAppDataPath = NULL;

		//HANDLE handle = ::LoadLibraryEx(L"WebView2Container.exe", NULL, LOAD_LIBRARY_AS_DATAFILE);
		HRSRC resourceHandle = ::FindResource(NULL, MAKEINTRESOURCE(IDR_HTML1), MAKEINTRESOURCE(RT_HTML));
		DWORD   size = ::SizeofResource(NULL, resourceHandle);
		HGLOBAL dataHandle = ::LoadResource(NULL, resourceHandle);
		// char* datac = nullptr;
		//LPTSTR  data = (LPTSTR) LockResource(dataHandle);
		// datac = (char*)LockResource(dataHandle);

		LPTSTR  data = (LPTSTR)LockResource(dataHandle);

		if (dataHandle == NULL) {
			MessageBox(
				nullptr,
				L"NIX",
				L"WebView2Runner",
				MB_ICONERROR);
		}
		else {
			//std::wstring datac(data, size);

			if (data==NULL) {
				MessageBox(
					nullptr,
					L"NIX2",
					L"WebView2Runner",
					MB_ICONERROR);
			}
			else {

			MessageBox(
				nullptr,
				LoadHtml().c_str(),
				L"titel1",
				MB_ICONERROR);
			}
		}

		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppDataPath)))
		{
			std::wstring html = LoadStringAsWstr(hInstance, 102);
			std::wstring path = std::wstring(localAppDataPath) + L"\\WebViewContainer";
			CoTaskMemFree((LPVOID)localAppDataPath);

			//SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);

			ShowWindow(hwnd, nCmdShow);
			UpdateWindow(hwnd);

			CreateCoreWebView2EnvironmentWithOptions(
				nullptr,
				&path[0],
				nullptr,
				Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
					[hwnd, html, data](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
						// Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
						env->CreateCoreWebView2Controller(hwnd, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
							[hwnd, html, data](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
								if (controller != nullptr) {
									webViewController = controller;
									webViewController->get_CoreWebView2(&webViewWindow);
								}

								// Add a few settings for the webview
								// The demo step is redundant since the values are the default settings
								ICoreWebView2Settings* Settings;
								webViewWindow->get_Settings(&Settings);
								Settings->put_IsScriptEnabled(TRUE);
								Settings->put_AreDefaultScriptDialogsEnabled(TRUE);
								Settings->put_IsWebMessageEnabled(TRUE);

								// Resize WebView to fit the bounds of the parent window
								RECT bounds;
								GetClientRect(hwnd, &bounds);
								webViewController->put_Bounds(bounds);

								// Schedule an async task to navigate to Bing
							   // webViewWindow->Navigate(L"https://www.bing.com/");
								webViewWindow->Navigate(L"res://WebView2Container.exe/html1.htm");
								webViewWindow->NavigateToString(LoadHtml().c_str());
								//webViewWindow->NavigateToString(&html[0]);//L"NIX");

								// Step 4 - Navigation events

								// Step 5 - Scripting

								// Step 6 - Communication between host and web content

								return S_OK;
							}).Get());
						return S_OK;
					}
				).Get()
			);

			// Run the message loop.

			MSG msg = { };
			while (GetMessage(&msg, NULL, 0, 0) > 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		}

		return 0;
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

std::wstring LoadStringFromResource(HINSTANCE instance, UINT stringID)
{
	WCHAR* pBuf = nullptr;
	int len = LoadStringW(
		instance,
		stringID,
		reinterpret_cast<LPWSTR>(&pBuf),
		0);

	return std::wstring(pBuf, len);
}

std::wstring LoadStringAsWstr(HINSTANCE hInstance, UINT uID)
{
	PCWSTR pws = nullptr;
	int cch = LoadStringW(hInstance, uID, reinterpret_cast<LPWSTR>(&pws), 0);
	return std::wstring(pws, cch);
}

std::wstring LoadHtml() {
	HRSRC resourceHandle = ::FindResource(NULL, MAKEINTRESOURCE(IDR_HTML1), MAKEINTRESOURCE(RT_HTML));
	
	if (resourceHandle==NULL) {
		return L"nix 1";
	}

	DWORD   size = ::SizeofResource(NULL, resourceHandle);

	if (size == NULL) {
		return L"nix 2";
	}

	HGLOBAL dataHandle = ::LoadResource(NULL, resourceHandle);

	if (dataHandle == NULL) {
		return L"nix 3";
	}

	const char* data = (char*) LockResource(dataHandle);

	if (data == NULL) {
		return L"nix 4";
	}

	int count = MultiByteToWideChar(CP_UTF8, 0, data, size, NULL, 0);

	if (count > 0) {
		std::wstring wstr(count, 0);
		MultiByteToWideChar(CP_UTF8, 0, data, size, &wstr[0], count);
		return wstr;
		return L"nix 5";
	}
	return L"nix 6";
}

std::string ConvertWideToUtf8(const std::wstring& wstr)
{
	int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
	std::string str(count, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
	return str;
}

std::wstring ConvertUtf8ToWide(const std::string& str)
{
	int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
	std::wstring wstr(count, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
	return wstr;
}