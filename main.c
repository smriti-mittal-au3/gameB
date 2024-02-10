//#pragma warning(push, 3)
//#pragma warning(disable : 4668)

#include <windows.h> 
#include <stdint.h>

//#pragma warning(pop)

#include "main.h"


BOOL gGameIsRunning = FALSE;
HWND gGameWindow;
GAMEBITMAPINFO gGameBitMap = { 0 };
MONITORINFO gMonitorInfo;


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(nShowCmd);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(hInstance);

    MSG Msg = { 0 };

    if (GameIsAlreadyRunning() == TRUE)
    {
        goto Exit;
    }

    if (CreateMainWindow() != ERROR_SUCCESS)
    {
        goto Exit;
    }


 
    gMonitorInfo.cbSize = sizeof(MONITORINFO);

    if (GetMonitorInfoA(MonitorFromWindow(gGameWindow, MONITOR_DEFAULTTOPRIMARY), &gMonitorInfo) == 0) {
        MessageBoxA(NULL, "Get Monitor Info Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    }
    
    int32_t MonitorWidth = gMonitorInfo.rcMonitor.right - gMonitorInfo.rcMonitor.left;
    int32_t MonitorHeight = gMonitorInfo.rcMonitor.bottom - gMonitorInfo.rcMonitor.top;

    SetWindowLongPtrA(gGameWindow, GWL_STYLE, (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_OVERLAPPEDWINDOW);

    SetWindowPos(
        gGameWindow, 
        HWND_TOP, 
        gMonitorInfo.rcMonitor.left, 
        gMonitorInfo.rcMonitor.top, 
        MonitorWidth, 
        MonitorHeight, 
        SWP_SHOWWINDOW
    );
   

    gGameBitMap.bitmapinfo.bmiHeader.biSize = sizeof(gGameBitMap.bitmapinfo.bmiHeader);
    gGameBitMap.bitmapinfo.bmiHeader.biPlanes = 1;


    gGameBitMap.bitmapinfo.bmiHeader.biWidth = GAME_RES_WIDTH;
    gGameBitMap.bitmapinfo.bmiHeader.biHeight = GAME_RES_HEIGHT;

    gGameBitMap.bitmapinfo.bmiHeader.biBitCount = GAME_BPP;
    gGameBitMap.bitmapinfo.bmiHeader.biCompression = BI_RGB;


    gGameBitMap.Memory = VirtualAlloc(NULL, GAME_DRAWING_AREA_MEMORY_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (gGameBitMap.Memory == NULL) {
        //Result = GetLastError();
        MessageBox(gGameWindow, "Memory not allocated!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto Exit;
    }

    memset(gGameBitMap.Memory, 0x00007F, GAME_DRAWING_AREA_MEMORY_SIZE);


    PIXEL32 Pixel = { 0 };
    Pixel.Blue = 0xFF;
    Pixel.Green = 0x00;
    Pixel.Red = 0x00;
    Pixel.Alpha = 0x00;

    for (int x = 0; x < GAME_RES_WIDTH * GAME_RES_HEIGHT; x++)
    {

        memcpy_s((PIXEL32*) gGameBitMap.Memory + x, sizeof(PIXEL32),  &Pixel, 4);
    }


    gGameIsRunning = TRUE;

    while (gGameIsRunning == TRUE)
    {
        while (PeekMessageA(&Msg, gGameWindow, 0, 0, TRUE) > 0)
        {
            TranslateMessage(&Msg);
            DispatchMessageA(&Msg);
        }


        RenderGameGraphics();
        // Now we can do other stuff, but why
        ProcessPlayerInput();

        Sleep(1);

    }


    goto Exit;

Exit:
    return(0);
}


LRESULT __stdcall WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = 0;
    switch (msg)
    {
    case WM_CLOSE:
        gGameIsRunning = FALSE;
        PostQuitMessage(0);
        break;
    case WM_DESTROY:
        break;
    default:
        Result = DefWindowProc(hwnd, msg, wParam, lParam);
        goto Exit;
        break;
    }

Exit:
    return Result;

}


DWORD CreateMainWindow(void)
{
    DWORD Result = ERROR_SUCCESS;
    WNDCLASSEXA wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0xFF, 0x00, 0xFF));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GAME_B_WINDOW_CLASS";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (RegisterClassEx(&wc) == 0)
    {
        Result = GetLastError();

        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        goto Exit;
    }



    gGameWindow = CreateWindowEx(
        0,
        wc.lpszClassName,
        "The title of my window",
        WS_OVERLAPPEDWINDOW | WS_OVERLAPPED | WS_HSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 420,
        NULL, NULL, GetModuleHandleA(NULL), NULL);

    if (gGameWindow == NULL)
    {
        Result = GetLastError();
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }

    ShowWindow(gGameWindow, TRUE);


Exit:
    return(Result);
}


BOOL GameIsAlreadyRunning(void)
{
    HANDLE Mutex = NULL;
    Mutex = CreateMutexA(NULL, FALSE, "GAME_B_WINDOW_CLASS");

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBox(NULL, "Already running!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return(TRUE);
    }

    return(FALSE);
}


VOID ProcessPlayerInput(void)
{
 
    int16_t EscapeKeyIsDown = GetAsyncKeyState(VK_ESCAPE);
    
    if (EscapeKeyIsDown)
    {
        SendMessage(gGameWindow, WM_CLOSE, 0, 0);
    }
}

void RenderGameGraphics(void)
{

    HDC DeviceContext = GetDC(gGameWindow);
   
  
    int DIBits = StretchDIBits(
        DeviceContext, 
        0, 
        0, 
        gMonitorInfo.rcMonitor.right - gMonitorInfo.rcMonitor.left, 
        gMonitorInfo.rcMonitor.bottom - gMonitorInfo.rcMonitor.top,
        //x-coordinate of buffer
        0, 
        0, 
        GAME_RES_WIDTH, 
        GAME_RES_HEIGHT, 
        gGameBitMap.Memory, 
        &gGameBitMap.bitmapinfo, 
        DIB_RGB_COLORS, 
        SRCCOPY);

    if (DIBits == 0) {
        MessageBoxA(NULL, "Stretch Bits Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    }

    //When to release ? Just now ..
    ReleaseDC(gGameWindow, DeviceContext);


}