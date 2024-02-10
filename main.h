#pragma once
#pragma warning(disable : 4820) // Disable warning about structure padding
#pragma warning(disable : 5045) // Disable warning about Spectre / Mitigation

#define GAME_RES_WIDTH	384

#define GAME_RES_HEIGHT	216

#define GAME_BPP		32

#define GAME_DRAWING_AREA_MEMORY_SIZE	(GAME_RES_WIDTH * GAME_RES_HEIGHT * (GAME_BPP / 8))


//#define int GAME_NAME = "GAME_B_";
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD CreateMainWindow(void);
VOID ProcessPlayerInput(void);
void RenderGameGraphics(void);
BOOL GameIsAlreadyRunning(void);

//what is DWORD man
typedef struct {
	BITMAPINFO bitmapinfo;
	//4 bytes padding
	void* Memory;
} GAMEBITMAPINFO;


typedef struct {
	uint8_t Blue;
	uint8_t Green;
	uint8_t Red;
	uint8_t Alpha;
} PIXEL32;


