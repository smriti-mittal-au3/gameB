#pragma once

//#define int GAME_NAME = "GAME_B_";
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD CreateMainWindow();
VOID ProcessPlayerInput(void);
void RenderGameGraphics(void);

//what is typedef
//what is DWORD man
typedef struct {
	//why this struct 
	BITMAPINFO bitmapinfo;
	
	//why cudn't bitmapInfo also have a memory variable
	//why is it a pointer .. or it's just a pointer .. an address '
	//pointer size depends on x86 or x64
	void* Memory;
} GAMEBITMAPINFO;