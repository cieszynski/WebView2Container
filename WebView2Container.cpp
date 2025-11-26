#ifndef UNICODE
#define UNICODE
#endif 

#include "WebView2Container.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)

{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Window Class";

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	// https://learn.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = CLASS_NAME;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			L"Call to RegisterClassEx failed!",
			L"WebView2Container",
			MB_ICONERROR);
	}

	// Create the window.

	HWND hWnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hWnd == NULL)
	{
		MessageBox(
			nullptr,
			L"Call to CreateWindow failed!",
			L"WebView2Runner",
			MB_ICONERROR);
		return 0;
	}

	PWSTR localAppDataPath = NULL;

	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppDataPath)))
	{
		std::wstring path = std::wstring(localAppDataPath) + L"\\WebViewContainer";
		CoTaskMemFree((LPVOID)localAppDataPath);

		//SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);

		CreateCoreWebView2EnvironmentWithOptions(
			nullptr,
			&path[0],
			nullptr,
			Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
				[hWnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
					// Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
					env->CreateCoreWebView2Controller(hWnd, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
						[hWnd, env](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
							if (controller != nullptr) {
								// https://stackoverflow.com/a/77772192
								controller->AddRef();
								webViewController = controller;
								webViewController->get_CoreWebView2(&webViewWindow);
							}

							ICoreWebView2Settings* Settings;
							webViewWindow->get_Settings(&Settings);
							Settings->put_AreDefaultContextMenusEnabled(FALSE);
#ifdef _DEBUG
							Settings->put_AreDevToolsEnabled(TRUE);
							Settings->put_IsStatusBarEnabled(TRUE);
#else
							Settings->put_IsStatusBarEnabled(FALSE);
							Settings->put_AreDevToolsEnabled(FALSE);
#endif

							// Resize WebView to fit the bounds of the parent window
							RECT bounds;
							GetClientRect(hWnd, &bounds);
							webViewController->put_Bounds(bounds);
							
							EventRegistrationToken token;

							webViewWindow->add_DocumentTitleChanged(
								Microsoft::WRL::Callback<ICoreWebView2DocumentTitleChangedEventHandler>(
									[hWnd](ICoreWebView2* webview, IUnknown* args) -> HRESULT {
										LPWSTR title;
										webview->get_DocumentTitle(&title);
										SetWindowTextW(hWnd, title);
										return S_OK;
									}).Get(), &token);

							webViewWindow->add_WindowCloseRequested(
								Microsoft::WRL::Callback<ICoreWebView2WindowCloseRequestedEventHandler>(
									[hWnd](ICoreWebView2* webview, IUnknown* args) {
										DestroyWindow(hWnd);
										return S_OK;
									}).Get(), &token);
							
							webViewWindow->AddWebResourceRequestedFilter(
								L"https://demo/*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
							webViewWindow->add_WebResourceRequested(
								Microsoft::WRL::Callback<ICoreWebView2WebResourceRequestedEventHandler>(
									[env](ICoreWebView2* webview, ICoreWebView2WebResourceRequestedEventArgs* args)->HRESULT {
										Microsoft::WRL::ComPtr<ICoreWebView2WebResourceResponse> response;
										Microsoft::WRL::ComPtr<ICoreWebView2SharedBuffer> sharedBuffer;
										Microsoft::WRL::ComPtr<IStream> stream;
										Microsoft::WRL::ComPtr<ICoreWebView2Environment12> env12;

										env->QueryInterface<ICoreWebView2Environment12>(&env12);
										std::wstring js = LoadHtmlFromResource(IDR_HTML1);
										// * sizeof(wchar_t)
										env12->CreateSharedBuffer(js.size()*2, &sharedBuffer);
										//env12->CreateSharedBuffer(32, &sharedBuffer);
										sharedBuffer->OpenStream(&stream);
										stream->Write(&js[0], js.size()*2, NULL);
										//stream->Write(L"/* <avascript */", 32, NULL);
										env->CreateWebResourceResponse(
											stream.Get(), 200, L"OK", L"Content-Type: text/html; charset=utf-16", &response);
										//stream->Commit(0);
										args->put_Response(response.Get());
										return S_OK;
									}).Get(), &token);

							std::wstring js = LoadHtmlFromResource(IDR_HTML2);

							webViewWindow->AddScriptToExecuteOnDocumentCreated(&js[0],
								Microsoft::WRL::Callback<ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler>(
									[](HRESULT error, PCWSTR id)->HRESULT {
										//std::wstring html = LoadHtmlFromResource(IDR_HTML1);
										//webViewWindow->NavigateToString(&html[0]);
										webViewWindow->Navigate(L"https://demo/");
										return S_OK;
									}).Get());
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

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		/*
		case WM_CLOSE:
			if (MessageBox(hWnd, L"Programm wirklich beenden?", L"Frage:", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
			{
				DestroyWindow(hWnd);
			}
			break;
		*/
	case WM_SIZE:
		if (webViewController != nullptr) {
			RECT bounds;
			GetClientRect(hWnd, &bounds);
			webViewController->put_Bounds(bounds);
		};
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

std::wstring LoadHtmlFromResource(WORD resId) {
	HRSRC resourceHandle = ::FindResource(NULL, MAKEINTRESOURCE(resId), MAKEINTRESOURCE(RT_HTML));

	if (resourceHandle == NULL) {
		return L"nix 1";
	}

	DWORD   size = ::SizeofResource(NULL, resourceHandle);

	if (size == NULL) {
		return L"nix 2";
	}

	HGLOBAL dataHandle = LoadResource(NULL, resourceHandle);

	if (dataHandle == NULL) {
		return L"nix 3";
	}

	const char* data = (char*)LockResource(dataHandle);

	if (data == NULL) {
		return L"nix 4";
	}

	int count = MultiByteToWideChar(CP_UTF8, 0, data, size, NULL, 0);

	if (count > 0) {
		std::wstring wstr(count, 0);
		MultiByteToWideChar(CP_UTF8, 0, data, size, &wstr[0], count);
		return wstr;
	}
	return L"Error: ";
}