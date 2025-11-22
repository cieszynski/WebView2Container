#ifndef UNICODE
#define UNICODE
#endif 

#include "WebView2Container.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
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

        SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppDataPath);

        if (localAppDataPath != NULL) {
            size_t localAppDataPathLength = wcslen(localAppDataPath) * sizeof(WCHAR);
            wcscat_s(localAppDataPath, localAppDataPathLength, L"\\wv2c");
        }

        SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);

        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);
        
        CreateCoreWebView2EnvironmentWithOptions(
            nullptr,
            localAppDataPath,
            nullptr,
            Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
                [hwnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                    // Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
                    env->CreateCoreWebView2Controller(hwnd, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [hwnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
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

                            webViewWindow->NavigateToString(L"NIX");

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

        CoTaskMemFree((LPVOID)localAppDataPath); // "The calling process is responsible for freeing this resource once it is no longer needed by calling CoTaskMemFree"
        localAppDataPath = NULL;
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