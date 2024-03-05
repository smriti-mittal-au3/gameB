#pragma once
#pragma warning(disable : 4820) // Disable warning about structure padding
#pragma warning(disable : 5045) // Disable warning about Spectre / Mitigation

#define GAME_RES_WIDTH	384

//why define works here
#define GAME_RES_HEIGHT	216

#define GAME_BPP		32

#define GAME_DRAWING_AREA_MEMORY_SIZE	(GAME_RES_WIDTH * GAME_RES_HEIGHT * (GAME_BPP / 8))

#define CALCULATE_AVG_FPS_EVERY_X_FRAMES	120

#define TARGET_MICROSECONDS_PER_FRAME 16667


//#define int GAME_NAME = "GAME_B_";
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD CreateMainWindow(void);
VOID ProcessPlayerInput(void);
void RenderGameGraphics(void);
BOOL GameIsAlreadyRunning(void);
void ClearScreen(__m128i * Pixel128);


typedef LONG(NTAPI* _NtQueryTimerResolution)(OUT PULONG MinimumResolution, OUT PULONG MaximumResolution, OUT PULONG CurrentResolution);
_NtQueryTimerResolution NtQueryTimerResolution;

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


typedef struct {
	uint64_t TotalFramesRendered;
	uint64_t TotalMicrosecondsElapsed;
	uint64_t PerfFrequency;

	float RawFPSAverage;
	float CookedFPSAverage;

	MONITORINFO MonitorInfo;
	int32_t MonitorWidth;
	int32_t MonitorHeight;

	LONG MinimumResolution;
	LONG MaximumResolution;
	LONG CurrentResolution;

	DWORD ProcessHandleCount;

	BOOL ShowDebugInfo;
} GAMEPERFDATA;


typedef struct PLAYER {
	int32_t WorldPosX;
	int32_t WorldPosY;
} PLAYER;