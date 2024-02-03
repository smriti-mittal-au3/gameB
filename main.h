#pragma once


#define GAME_RES_WIDTH	384

#define GAME_RES_HEIGHT	216

#define GAME_BPP		32

#define GAME_DRAWING_AREA_MEMORY_SIZE	(GAME_RES_WIDTH * GAME_RES_HEIGHT * (GAME_BPP / 8))


//#define int GAME_NAME = "GAME_B_";
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD CreateMainWindow();
VOID ProcessPlayerInput(void);
void RenderGameGraphics(void);

//what is DWORD man
typedef struct {
	BITMAPINFO bitmapinfo;
	//why is it a pointer .. or it's just a pointer .. an address '
	void* Memory;
} GAMEBITMAPINFO;