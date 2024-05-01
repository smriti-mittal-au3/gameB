#pragma once
#pragma warning(disable : 4820) // Disable warning about structure padding
#pragma warning(disable : 5045) // Disable warning about Spectre / Mitigation

#define GAME_RES_WIDTH	384

//why define works here
#define GAME_RES_HEIGHT	216

#define GAME_BPP		32

#define GAME_DRAWING_AREA_MEMORY_SIZE	(GAME_RES_WIDTH * GAME_RES_HEIGHT * (GAME_BPP / 8))

#define CALCULATE_AVG_FPS_EVERY_X_FRAMES	120

#define TARGET_MICROSECONDS_PER_FRAME 16667ULL

#define SUIT_0 0
#define SUIT_1 1
#define SUIT_2 2

#define FACING_DOWN_0	0

#define FACING_DOWN_1	1

#define FACING_DOWN_2	2

#define FACING_LEFT_0	3

#define FACING_LEFT_1	4

#define FACING_LEFT_2	5

#define FACING_RIGHT_0	6

#define FACING_RIGHT_1	7

#define FACING_RIGHT_2	8

#define FACING_UPWARD_0	9

#define FACING_UPWARD_1	10

#define FACING_UPWARD_2	11


//#define int GAME_NAME = "GAME_B_";
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD CreateMainWindow(void);
VOID ProcessPlayerInput(void);
void RenderGameGraphics(void);
BOOL GameIsAlreadyRunning(void);
void ClearScreen(__m128i * Pixel128);

DWORD Load32BPPBitmapFromFile(_In_ char* FileName, _Inout_ GAMEBITMAPINFO * GameBitmap);
DWORD InitializeHero(void);

typedef LONG(NTAPI* _NtQueryTimerResolution)(OUT PULONG MinimumResolution, OUT PULONG MaximumResolution, OUT PULONG CurrentResolution);
_NtQueryTimerResolution NtQueryTimerResolution;

//what is DWORD man
typedef struct { 
	BITMAPINFO Bitmapinfo;
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
	PROCESS_MEMORY_COUNTERS_EX MemInfo;

	double CPUPercentage;

	SYSTEM_INFO SystemInfo;

	BOOL ShowDebugInfo;
} GAMEPERFDATA;


typedef struct HERO {
	int32_t WorldPosX;
	int32_t WorldPosY;
	GAMEBITMAPINFO Sprite[3][12];
} HERO;