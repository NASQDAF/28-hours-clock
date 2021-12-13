#include <windows.h>
#include <time.h>
#include <string>
#include <thread>
#include <chrono>

#define ClockLabel 6004
static HWND Label;
unsigned short hour = 0;
SYSTEMTIME lt;
std::tm td{};
int today = td.tm_wday;
uint16_t h = 0;
inline short get28hour() {
	if (lt.wHour == 0)			today = td.tm_wday;
	if (lt.wHour < 4 * today)	hour = lt.wHour + (24 - 4 * today);
	else						hour = lt.wHour - 4 * today;
	return hour;
}
void update_Time() {
	while (1) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		GetLocalTime(&lt);
		if (lt.wMinute == 0)
			h = get28hour();
		SetWindowTextA(Label, (std::to_string(h) + ":" + std::to_string(lt.wMinute)).c_str());
	}
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {
	const wchar_t CLASS_NAME[] = L"28HC";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);
	HWND hwnd = CreateWindowEx(WS_EX_TOPMOST| WS_EX_TOOLWINDOW,
		CLASS_NAME,
		L"28 Hours Clock", 
		WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		50, 20, 
		NULL, NULL, hInstance, NULL);
	if (hwnd == NULL)return 0;
	ShowWindow(hwnd, nCmdShow);
	MSG Resp = {};
	GetLocalTime(&lt);
	h = get28hour();
	std::thread t([&]() {update_Time(); });
	t.detach();
	while (GetMessage(&Resp, NULL, 0, 0)) {
		TranslateMessage(&Resp);
		DispatchMessage(&Resp);
	}return 0;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CREATE:
			Label = CreateWindow(L"STATIC", L"00:00", 
				WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
				1, 1, 50, 20, hwnd, (HMENU)ClockLabel, NULL, NULL);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_INITDIALOG:
			return true;
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			EndPaint(hwnd, &ps);
		}
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
