#define _CRT_SECURE_NO_WARNINGS
#define ClockLabel	6004
#include <windows.h>
#include <time.h>
#include <string>
#include <thread>
#include <chrono>

static HWND Label;
SYSTEMTIME lt;
time_t rawtime;
tm * td;
uint16_t today, h = 0;

int get28hour() {
	 if (lt.wHour == 0) {
		 time(&rawtime);
		 td = localtime(&rawtime);
		 today = (td->tm_wday + 6)%7;
	 }
	return (today * 24 + lt.wHour) % 28;
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
	wc.style = CS_DBLCLKS;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(NULL, IDC_HAND);
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
	time(&rawtime);
	td = localtime(&rawtime);
	today = (td->tm_wday + 6) % 7;
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
		case WM_LBUTTONDBLCLK:
			if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK)
				exit(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}