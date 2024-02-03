#pragma warning(push, 3)
//#pragma warning(disable : 4668)
#include <windows.h> 
#pragma warning(pop)

#include "main.h"


BOOL gGameIsRunning = FALSE;
HWND gGameWindow;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

    MSG Msg = { 0 };

    if (GameIsAlreadyRunning() == TRUE)
    {
        goto Exit;
    }

    if (CreateMainWindow() != ERROR_SUCCESS)
    {
        goto Exit;
    }

    gGameIsRunning = TRUE;

    while (gGameIsRunning == TRUE)
    {
        while (PeekMessageA(&Msg, gGameWindow, 0, 0, TRUE) > 0)
        {
            TranslateMessage(&Msg);
            DispatchMessageA(&Msg);
        }

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
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GAME_B_WINDOW_CLASS";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (RegisterClassEx(&wc) == 0)
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        Result = GetLastError();
        goto Exit;
    }



    gGameWindow = CreateWindowEx(
        WS_EX_CLIENTEDGE,
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


VOID ProcessPlayerInput()
{
    unsigned int Overflow = 0xffffffff;
    unsigned char cx = 255;
    cx = 255 + 2;
    char xx = -128;
    xx = -128 - 1;
 
    SHORT EscapeKeyIsDown = GetAsyncKeyState(VK_ESCAPE);
    
    if (EscapeKeyIsDown)
    {
        SendMessage(gGameWindow, WM_CLOSE, 0, 0);
    }
}

void RenderGameGraphics(void)
{
    //void * Memory;

    GAMEBITMAPINFO gGameBitMap = { 0 };

    //is it the size of bmiHeader or just bitmapInfo
    //earlier we did sizeof the data struct but ..
    //how will it help computer know the size now but .. size of this new data structure GAMEBITMAPINFO
    gGameBitMap.bitmapinfo.bmiHeader.biSize = sizeof(gGameBitMap.bitmapinfo.bmiHeader);
    gGameBitMap.bitmapinfo.bmiHeader.biPlanes = 1;
    
    //aspect ratio = 16:9
    //no. of pixels 
    gGameBitMap.bitmapinfo.bmiHeader.biWidth = 256;
    gGameBitMap.bitmapinfo.bmiHeader.biHeight = 224;

    gGameBitMap.bitmapinfo.bmiHeader.biBitCount = 16;

    //why void * what is that 
    //what is MEM_PHYSICAL =  0x00400000
    //VirtualAlloc vs malloc
    // /8 two times
    gGameBitMap.Memory = VirtualAlloc(NULL, 256 * 16 / 8 * 224 * 16 / 8);

}