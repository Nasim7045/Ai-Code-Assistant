#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include <string.h>

#define GEMINI_API_KEY "API-key"
#define API_URL "https://your-gemini-endpoin"

// Function to send HTTP request using WinINet API
void sendHttpRequest(const char *userInput) {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    char buffer[4096];
    char request[1024];
    const char *headers = "Content-Type: application/x-www-form-urlencoded\r\n";
    
    // Create the HTTP request body
    snprintf(request, sizeof(request), "user_input=%s", userInput);

    // Initialize the WinINet API
    hInternet = InternetOpen("GeminiStealth", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        MessageBox(NULL, "Failed to initialize WinINet.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Establish a connection to the Gemini API server
    hConnect = InternetOpenUrl(hInternet, API_URL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        MessageBox(NULL, "Failed to connect to Gemini API.", "Error", MB_OK | MB_ICONERROR);
        InternetCloseHandle(hInternet);
        return;
    }

    // Send the request to Gemini API with POST method, headers, and request body
    BOOL result = HttpSendRequestA(hConnect, headers, strlen(headers), (LPVOID)request, strlen(request));
    if (!result) {
        MessageBox(NULL, "Failed to send HTTP request.", "Error", MB_OK | MB_ICONERROR);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return;
    }

    // Read the response
    ZeroMemory(buffer, sizeof(buffer));
    if (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead)) {
        buffer[bytesRead] = '\0';  // Null-terminate the response
        printf("Response from Gemini API: %s\n", buffer);
        // Here you can display the result in your window
    }

    // Clean up
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}

// Function to handle input and output
void processInput(HWND hwndEdit) {
    char userInput[1024];
    // Get user input from the window (Assuming input from hwndEdit)
    GetWindowText(hwndEdit, userInput, sizeof(userInput));
    
    // Send the input to the Gemini API and process the response
    sendHttpRequest(userInput);
}

// Window Procedure to handle events
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndEdit, hwndButton;
    
    switch (msg) {
        case WM_CREATE:
            // Create the text input box
            hwndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
                                      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
                                      10, 50, 500, 300, hwnd, NULL, NULL, NULL);
            // Create the button
            hwndButton = CreateWindow("BUTTON", "Process", 
                                      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                      10, 10, 100, 30, hwnd, (HMENU)1, NULL, NULL);
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                // Process the input when the button is pressed
                processInput(hwndEdit);
            }
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Function to initialize and register the window
void createStealthWindow(HINSTANCE hInstance, int nCmdShow) {
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, 
                     hInstance, LoadIcon(NULL, IDI_APPLICATION), 
                     LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 1), 
                     NULL, "StealthApp", LoadIcon(NULL, IDI_APPLICATION)};
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, "StealthApp", 
                               "Stealth Gemini AI", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
                               600, 400, NULL, NULL, hInstance, NULL);

    // Make window hidden from Alt+Tab and Taskbar
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
}

// Main Function to start the app
int main() {
    // Hide the CMD window
    HWND hwndConsole = GetConsoleWindow();
    ShowWindow(hwndConsole, SW_HIDE);  // Hide the console window

    // Initialize the window class and window
    HINSTANCE hInstance = GetModuleHandle(NULL);
    createStealthWindow(hInstance, SW_SHOW);

    // Enter the message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
