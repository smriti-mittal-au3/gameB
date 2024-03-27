//#pragma warning(push, 3)
//#pragma warning(disable : 4668)

#include <windows.h> 
#include <stdint.h>
#include <stdio.h>
#include <emmintrin.h>
#include <Psapi.h>

//#pragma warning(pop)

#include "main.h"


BOOL gGameIsRunning = FALSE;
HWND gGameWindow;
GAMEBITMAPINFO gGameBitMap = { 0 };
GAMEPERFDATA gGamePerformanceData = { 0 };
PLAYER gPlayer;



int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(nShowCmd);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(hInstance);

    MSG Msg = { 0 };
    int64_t FrameStart = 0;
    int64_t FrameEnd = 0;
    int64_t ElapsedMicrosecondsPerFrame = 0;
    int64_t ElapsedMicrosecondsPerFrameAccumulatorCooked = 0;
    int64_t ElapsedMicrosecondsPerFrameAccumulatorRaw = 0;

    TIMECAPS t = { 0 };
    //timeGetDevCaps();

    if (GameIsAlreadyRunning() == TRUE)
    {
        goto Exit;
    }

    if (CreateMainWindow() != ERROR_SUCCESS)
    {
        goto Exit;
    }


 
    gGamePerformanceData.MonitorInfo.cbSize = sizeof(MONITORINFO);
    gGamePerformanceData.ShowDebugInfo = TRUE;

    if (GetMonitorInfoA(MonitorFromWindow(gGameWindow, MONITOR_DEFAULTTOPRIMARY), &gGamePerformanceData.MonitorInfo) == 0) {
        MessageBoxA(NULL, "Get Monitor Info Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    }
    
    int32_t MonitorWidth = gGamePerformanceData.MonitorInfo.rcMonitor.right - gGamePerformanceData.MonitorInfo.rcMonitor.left;
    int32_t MonitorHeight = gGamePerformanceData.MonitorInfo.rcMonitor.bottom - gGamePerformanceData.MonitorInfo.rcMonitor.top;


    SetWindowLongPtrA(gGameWindow, GWL_STYLE,  WS_VISIBLE);

    SetWindowPos(
        gGameWindow, 
        HWND_TOP, 
        gGamePerformanceData.MonitorInfo.rcMonitor.left,
        gGamePerformanceData.MonitorInfo.rcMonitor.top,
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

    __m128i Pixel128 = { 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00 };

    gPlayer.WorldPosX = 15;
    gPlayer.WorldPosY = 15;

    

    gGameIsRunning = TRUE;

    QueryPerformanceFrequency(&gGamePerformanceData.PerfFrequency);

    HINSTANCE NTDllModuleHandle;

    NTDllModuleHandle = GetModuleHandle("ntdll.dll");

    BOOL ProcessHandleCount = GetProcessHandleCount(GetCurrentProcess(), &gGamePerformanceData.ProcessHandleCount);
    GetSystemInfo(&gGamePerformanceData.SystemInfo);

    //in bytes ?

    if (NTDllModuleHandle == NULL)
    {
        MessageBoxA(NULL, "Couldn't load ntdll.dll", "Error!", MB_ICONEXCLAMATION | MB_OK);

        goto Exit;
    }

    NtQueryTimerResolution = (_NtQueryTimerResolution)GetProcAddress(NTDllModuleHandle, "NtQueryTimerResolution");

    if (NULL != NtQueryTimerResolution)
    {

        NtQueryTimerResolution(&gGamePerformanceData.MaximumResolution, &gGamePerformanceData.MinimumResolution, &gGamePerformanceData.CurrentResolution);
    }
  

    while (gGameIsRunning == TRUE)
    {
        QueryPerformanceCounter(&FrameStart);
 
        while (PeekMessageA(&Msg, gGameWindow, 0, 0, TRUE) > 0)
        {
            TranslateMessage(&Msg);
            DispatchMessageA(&Msg);
        }


        ClearScreen(&Pixel128);

        RenderGameGraphics();
        // Now we can do other stuff, but why
        ProcessPlayerInput();

        QueryPerformanceCounter((LARGE_INTEGER*)&FrameEnd);

        ElapsedMicrosecondsPerFrame = FrameEnd - FrameStart;

        ElapsedMicrosecondsPerFrame *= 1000000;

        ElapsedMicrosecondsPerFrame /= gGamePerformanceData.PerfFrequency;

        gGamePerformanceData.TotalFramesRendered++;

        ElapsedMicrosecondsPerFrameAccumulatorRaw += ElapsedMicrosecondsPerFrame;

        while (ElapsedMicrosecondsPerFrame < TARGET_MICROSECONDS_PER_FRAME)
        {
            
            ElapsedMicrosecondsPerFrame = FrameEnd - FrameStart;

            ElapsedMicrosecondsPerFrame *= 1000000;

            ElapsedMicrosecondsPerFrame /= gGamePerformanceData.PerfFrequency;

            QueryPerformanceCounter((LARGE_INTEGER*)&FrameEnd);

            if (ElapsedMicrosecondsPerFrame < TARGET_MICROSECONDS_PER_FRAME  - gGamePerformanceData.CurrentResolution / 10.0f)
            {
                //Sleep(1);
            }
        }

        ElapsedMicrosecondsPerFrameAccumulatorCooked += ElapsedMicrosecondsPerFrame;

        if ((gGamePerformanceData.TotalFramesRendered % CALCULATE_AVG_FPS_EVERY_X_FRAMES) == 0)
        {
            K32GetProcessMemoryInfo(GetCurrentProcess(), &gGamePerformanceData.MemInfo, sizeof(PROCESS_MEMORY_COUNTERS));

            GetProcessTimes(GetCurrentProcess(), &gGamePerformanceData.CreationTime, &gGamePerformanceData.ExitTime, &gGamePerformanceData.KernelTime, &gGamePerformanceData.UserTime);
            GetSystemTimeAsFileTime((FILETIME*)&gGamePerformanceData.CurrentSystemTime);
            gGamePerformanceData.CPUPercentage = (double)(gGamePerformanceData.UserTime + gGamePerformanceData.KernelTime \
                - gGamePerformanceData.PreviousKernelTime - \
                gGamePerformanceData.PreviousUserTime);
            gGamePerformanceData.CPUPercentage /= (gGamePerformanceData.CurrentSystemTime - \
                gGamePerformanceData.PreviousSystemTime);
            gGamePerformanceData.CPUPercentage /= gGamePerformanceData.SystemInfo.dwNumberOfProcessors;

            gGamePerformanceData.CPUPercentage *= 100;
            gGamePerformanceData.RawFPSAverage = 1.0f / ((ElapsedMicrosecondsPerFrameAccumulatorRaw / CALCULATE_AVG_FPS_EVERY_X_FRAMES) * 0.000001f);
            gGamePerformanceData.CookedFPSAverage = 1.0f / ((ElapsedMicrosecondsPerFrameAccumulatorCooked / CALCULATE_AVG_FPS_EVERY_X_FRAMES) * 0.000001f);

            ElapsedMicrosecondsPerFrameAccumulatorRaw = 0;
            ElapsedMicrosecondsPerFrameAccumulatorCooked = 0;

            gGamePerformanceData.PreviousKernelTime = gGamePerformanceData.KernelTime;
            gGamePerformanceData.PreviousUserTime = gGamePerformanceData.UserTime;
            gGamePerformanceData.PreviousSystemTime = gGamePerformanceData.CurrentSystemTime;

        }
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
    int16_t DebugKeyIsDown = GetAsyncKeyState(VK_F11);
    int16_t RightKeyIsDown = GetAsyncKeyState(VK_RIGHT) | GetAsyncKeyState('D');
    int16_t LeftKeyIsDown = GetAsyncKeyState(VK_LEFT) | GetAsyncKeyState('A');
    int16_t UpKeyIsDown = GetAsyncKeyState(VK_UP) | GetAsyncKeyState('W');
    int16_t DownKeyIsDown = GetAsyncKeyState(VK_DOWN) | GetAsyncKeyState('X');

    static BOOL DebugKeyWasDown = 0;

    if (EscapeKeyIsDown)
    {
        SendMessage(gGameWindow, WM_CLOSE, 0, 0);
    }

    if (DebugKeyIsDown && DebugKeyWasDown == FALSE) 
    {
        //don't toggle if still down'
        gGamePerformanceData.ShowDebugInfo = !gGamePerformanceData.ShowDebugInfo; 
    }
    DebugKeyWasDown = DebugKeyIsDown;

    // x will circle back. but y wil crash hmmm why so
    if (RightKeyIsDown)
    {
        gPlayer.WorldPosX++;
    }

    if (LeftKeyIsDown)
    {
        gPlayer.WorldPosX--;
    }

    if (UpKeyIsDown)
    {
        gPlayer.WorldPosY--;
    }

    if (DownKeyIsDown)
    {
        gPlayer.WorldPosY++  ;
    }


}

void RenderGameGraphics(void)
{


    PIXEL32 Pixel32 = { 0xff, 0xff, 0xff, 0xff };
    int32_t StartingPixel = GAME_RES_WIDTH * GAME_RES_HEIGHT - GAME_RES_WIDTH * gPlayer.WorldPosY - (GAME_RES_WIDTH - gPlayer.WorldPosX);

    for (int16_t x = 0; x < 16; x++)
    {
        for (int16_t y = gPlayer.WorldPosY; y < gPlayer.WorldPosY + 16; y++)
        {

            memcpy_s((PIXEL32*)gGameBitMap.Memory + StartingPixel + x - (uintptr_t)GAME_RES_WIDTH * y, sizeof(PIXEL32), &Pixel32, sizeof(PIXEL32));

        }
    }
    HDC DeviceContext = GetDC(gGameWindow);
   
  
    int DIBits = StretchDIBits(
        DeviceContext, 
        0, 
        0, 
        gGamePerformanceData.MonitorInfo.rcMonitor.right - gGamePerformanceData.MonitorInfo.rcMonitor.left,
        gGamePerformanceData.MonitorInfo.rcMonitor.bottom - gGamePerformanceData.MonitorInfo.rcMonitor.top,
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



    if (gGamePerformanceData.ShowDebugInfo == TRUE)
    {

        (HFONT)SelectObject(DeviceContext, (HFONT)GetStockObject(ANSI_FIXED_FONT));

        char Buffer[64] = { 0 };

        sprintf_s(Buffer, _countof(Buffer), "Min Res: %.02f\n",
            gGamePerformanceData.MinimumResolution / 10000.0f);

        TextOutA(DeviceContext, 0, 0, Buffer, (int)strlen(Buffer));

        sprintf_s(Buffer, _countof(Buffer), "Max Res: %.02f\n",
            gGamePerformanceData.MaximumResolution / 10000.0f);

        TextOutA(DeviceContext, 0, 14, Buffer, (int)strlen(Buffer));

        sprintf_s(Buffer, _countof(Buffer), "Current Res: %.02f\n",
            gGamePerformanceData.CurrentResolution / 10000.0f);

        TextOutA(DeviceContext, 0, 28, Buffer, (int)strlen(Buffer));

        sprintf_s(Buffer, _countof(Buffer), "Cooked FPS: %.02f\n",
            gGamePerformanceData.CookedFPSAverage);

        TextOutA(DeviceContext, 0, 42, Buffer, (int)strlen(Buffer));

        sprintf_s(Buffer, _countof(Buffer), "Raw FPS: %.02f\n",
            gGamePerformanceData.RawFPSAverage);

        TextOutA(DeviceContext, 0, 56, Buffer, (int)strlen(Buffer));
        
        // not a float but sprintf_s takes in flat, hence, just typecasting
        sprintf_s(Buffer, _countof(Buffer), "X World Pos: %.02f\n",
                (float)gPlayer.WorldPosX);

        TextOutA(DeviceContext, 0, 70, Buffer, (int)strlen(Buffer));

        sprintf_s(Buffer, _countof(Buffer), "Handle Count: %lu\n",
            gGamePerformanceData.ProcessHandleCount);

        TextOutA(DeviceContext, 0, 84, Buffer, (int)strlen(Buffer));

        sprintf_s(Buffer, _countof(Buffer), "MemInfo: %lu \n", gGamePerformanceData.MemInfo.PagefileUsage / 1024);

        TextOutA(DeviceContext, 0, 98, Buffer, (int)strlen(Buffer));

        sprintf_s(Buffer, _countof(Buffer), "CPU Usage: %0.02f \n", gGamePerformanceData.CPUPercentage);

        TextOutA(DeviceContext, 0, 114, Buffer, (int)strlen(Buffer));


    }

    //When to release ? Just now ..
    ReleaseDC(gGameWindow, DeviceContext);


}

void ClearScreen(__m128i * Pixel128)
{
    for (int x = 0; x < GAME_RES_WIDTH * GAME_RES_HEIGHT / 4; x++)
    {
        _mm_storeu_si128((__m128i*)gGameBitMap.Memory + x, *Pixel128);
        //memcpy_s((PIXEL32*) gGameBitMap.Memory + x, sizeof(PIXEL32),  &Pixel, 4);
    }
}