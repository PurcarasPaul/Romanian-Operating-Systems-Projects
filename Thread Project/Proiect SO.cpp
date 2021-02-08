#undef UNICODE
#include <windows.h>
#include <process.h>
#include "resource.h"

HANDLE MutexCount;

typedef struct
{
    HWND hwnd;
    HANDLE hEvent;
    int contor;
    BOOL Buton1;
    BOOL Buton2;
}PARAMS, * PPARAMS;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg,
    WPARAM wParam, LPARAM lParam);

BOOL dlgActive = FALSE; //variabila pentru caseta de dialog
HWND hwndMain; //variabila handle a ferestrei principale

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    static char szAppName[] = "Multi2";
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

    hwnd = CreateWindow(szAppName, "Multitasking Demo",
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

void Thread1(PVOID pvoid)
{
    HANDLE MutexCount = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "MutexCount");
    PPARAMS pparams = (PPARAMS)pvoid;
    while (TRUE)
    {
        if (pparams->Buton2 == TRUE)
            break;
        if (pparams->Buton1 == TRUE)
            SetEvent(pparams->hEvent);
        WaitForSingleObject(MutexCount, INFINITE);
        if (pparams->contor < 0)
            pparams->contor = 0;
        Sleep(1000);
        SetDlgItemInt(pparams->hwnd, IDC_SHOW1, pparams->contor+=10, FALSE);
        ReleaseMutex(MutexCount);
    }
    _endthread();
}

void Thread2(PVOID pvoid)
{
    HANDLE MutexCount = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "MutexCount");
    PPARAMS pparams = (PPARAMS)pvoid;
    while (TRUE)
    {
        if (pparams->Buton1 == TRUE)
            break;
        if (pparams->Buton2 == TRUE)
            SetEvent(pparams->hEvent);
        WaitForSingleObject(MutexCount, INFINITE);
        if (pparams->contor < 0)
            pparams->contor = 0;
        Sleep(1000);
        SetDlgItemInt(pparams->hwnd, IDC_SHOW2, pparams->contor += 20, FALSE);
        ReleaseMutex(MutexCount);
    }
    _endthread();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;
    switch (iMsg)
    {
    case WM_CREATE:
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        if (!dlgActive) {
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG),
                hwnd, DlgProc);
            PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    static PARAMS params;
    HFONT hFont;
    HWND hEditCtrl;
    switch (iMsg) {
    case WM_INITDIALOG:
        MutexCount = CreateMutex(NULL, FALSE, "MutexCount");
        hFont = CreateFont(80, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
            VARIABLE_PITCH, TEXT("Times New Roman"));
        hEditCtrl = GetDlgItem(hDlg, IDC_SHOW1);
        SendMessage(hEditCtrl, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SetDlgItemInt(hDlg, IDC_SHOW1, params.contor, FALSE);

        hFont = CreateFont(80, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
            VARIABLE_PITCH, TEXT("Times New Roman"));
        hEditCtrl = GetDlgItem(hDlg, IDC_SHOW2);
        SendMessage(hEditCtrl, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SetDlgItemInt(hDlg, IDC_SHOW2, params.contor, FALSE);

        params.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        params.hwnd = hDlg;
        params.Buton1 = FALSE;
        params.Buton2 = FALSE;

        return TRUE;
    case WM_CLOSE:
        SetEvent(params.hEvent);
        dlgActive = FALSE;
        EndDialog(hDlg, 0);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BUTTON1:
            _beginthread(Thread1, 0, &params);
            params.Buton1 = TRUE;
            params.Buton2 = FALSE;
            EnableWindow(GetDlgItem(hDlg, IDC_BUTTON1), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_BUTTON2), TRUE);
            return TRUE;
        case IDC_BUTTON2:
            _beginthread(Thread2, 0, &params);
            params.Buton2 = TRUE;
            params.Buton1 = FALSE;
            EnableWindow(GetDlgItem(hDlg, IDC_BUTTON1), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDC_BUTTON2), FALSE);
            return TRUE;
        case ID_CANCEL:
            dlgActive = FALSE;
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}