/*----------------------------------------
   BIGJOB.C -- Multithreading Demo

  ----------------------------------------*/
#undef UNICODE
#include <windows.h>
#include <math.h>
#include <process.h>

#define REP              10000

#define STATUS_ABORTED    0
#define STATUS_WORKING    1
#define STATUS_DONE       2
#define STATUS_NOTSTARTED 3

#define WM_CALC_DONE     (WM_USER + 0)
#define WM_CALC_ABORTED  (WM_USER + 1)

typedef struct
{
	HWND   hwnd;
	HANDLE hEvent;
	BOOL   bContinue;
	BOOL   bStop;
}
PARAMS, * PPARAMS;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static char szAppName[] = "BigJob";
	HWND        hwnd;
	MSG         msg;
	WNDCLASSEX  wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(szAppName, "Multithreading Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void Thread(PVOID pvoid)
{
	double  A = 1.0;
	INT     i, temp = 0;
	LONG    lTime;
	PPARAMS pparams;
	char title[200];
	pparams = (PPARAMS)pvoid;

	while (TRUE)
	{
		WaitForSingleObject(pparams->hEvent, INFINITE);

		if (pparams->bStop == TRUE) break;
		lTime = GetCurrentTime();

		for (i = temp; i < REP && pparams->bContinue; i++) {
			A = tan(atan(exp(log(sqrt(A * A))))) + 1.0;
			wsprintf(title, "Iteratia: %d", i);
			SetWindowText(pparams->hwnd, title);
			temp = i;
		}

		if (i == REP)
		{
			lTime = GetCurrentTime() - lTime;
			SendMessage(pparams->hwnd, WM_CALC_DONE, 0, lTime);
			temp = 0;
		}
		else
			SendMessage(pparams->hwnd, WM_CALC_ABORTED, 0, 0);
	}

	_endthread();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	const static char* szMessage[] = { "Intrerupt. Pentru reluare, clic buton stanga",
								  "Lucrez (butonul drept pentru intrerupere)",
								  "Am terminat. Pentru reluare, clic buton stanga",
								  "Ready. Pentru start, clic buton stanga" };
	static HANDLE hEvent;
	static INT    iStatus;
	static LONG   lTime;
	static PARAMS params;
	char          szBuffer[64];
	HDC           hdc;
	PAINTSTRUCT   ps;
	RECT          rect;

	switch (iMsg)
	{
	case WM_CREATE:
		hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		ResetEvent(hEvent);
		params.hwnd = hwnd;
		params.hEvent = hEvent;
		params.bContinue = FALSE;
		params.bStop = FALSE;

		iStatus = STATUS_NOTSTARTED;

		_beginthread(Thread, 0, &params);

		return 0;

	case WM_LBUTTONDOWN:
		if (iStatus == STATUS_WORKING)
		{
			MessageBeep(0);
			return 0;
		}

		iStatus = STATUS_WORKING;

		params.bContinue = TRUE;

		SetEvent(hEvent);

		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_RBUTTONDOWN:
		ResetEvent(hEvent);
		params.bContinue = FALSE;
		return 0;

	case WM_CALC_DONE:
		ResetEvent(hEvent);
		lTime = lParam;
		iStatus = STATUS_DONE;
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_CALC_ABORTED:
		iStatus = STATUS_ABORTED;
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);

		wsprintf(szBuffer, szMessage[iStatus], REP, lTime);

		DrawText(hdc, szBuffer, -1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		params.bContinue = FALSE;
		params.bStop = TRUE;
		SetEvent(hEvent);

		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
