#undef UNICODE
#include <windows.h>
#include "resource.h"

#define WM_PleaseWork WM_APP + 0x100
HWND hDialog,hwnd;
LPCTSTR GetFromClip = "GetFromClipboard";
static UINT WM_GETFROMCLIP;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg,
    WPARAM wParam, LPARAM lParam);

BOOL dlgActive = FALSE; //variabila pentru caseta de dialog
HWND hwndMain; //variabila handle a ferestrei principale

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    static char szAppName[] = "GetClip";
    HWND        hwnd;
    MSG         msg;
    WNDCLASSEX  wndclass;

    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;  //optiuni pentru stilul clasei 
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    // incarca pictograma fisierelor de tip aplicatie
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); // incarca cursorul sageata  
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    // fondul ferstrei de culoare alba     
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&wndclass);

    hwnd = CreateWindow(szAppName, //numele clasei inregistrat cu RegisterClass
        "Get From Clipboard",    // text pentru bara de titlu a ferestrei
        WS_OVERLAPPEDWINDOW,   // stilul ferestrei
        CW_USEDEFAULT,      // pozitia orizontala implicitã
        CW_USEDEFAULT,      // pozitia verticala implicita
        CW_USEDEFAULT,       // latimea implicita
        CW_USEDEFAULT,       // inaltimea implicita
        NULL,               // handle-ul ferestrei parinte
        NULL,               // handle-ul meniului ferestrei
        hInstance,          // proprietara ferestrei 
        NULL);


    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE);
    // schimba dimensiunea, pozitia si ordinea z a ferestrei copil, a ferestrei pop-up
    ShowWindow(hwnd, SW_HIDE);
    UpdateWindow(hwnd);
    hwndMain = hwnd;



    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;
    switch (iMsg)
    {
    case WM_CREATE: // operatiile ce se executa la crearea ferestrei
       // se creaza caseta de dialog
        WM_GETFROMCLIP = RegisterWindowMessage(GetFromClip);
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        if (!dlgActive) {
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG),
                hwnd, DlgProc);
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            // insereaza un nou mesaj nou in coada de asteptare
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0); // insereaza un mesaj de incheiere 
        return 0;
    }
    if (iMsg == WM_GETFROMCLIP)
    {
        SendMessage(hDialog, WM_PleaseWork, 0, 0);
        return TRUE;
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////


BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    BOOL bAvailable;
    HANDLE hClipMemory;
    char* pMyCopy, * pClipMemory;
    static char szBufferPaint[100] = "Astept msg...";
    switch (iMsg) 
    {
    case WM_INITDIALOG:
        // setari initiale	     
        hDialog = hDlg;
        WM_GETFROMCLIP = RegisterWindowMessage(GetFromClip);
        SetDlgItemText(hDlg, IDC_EDIT, szBufferPaint);
        return TRUE;
    case WM_CLOSE:
        dlgActive = FALSE;
        EndDialog(hDlg, 0);
        return TRUE;
    case WM_PleaseWork:
    {
        bAvailable = IsClipboardFormatAvailable(CF_TEXT);
        if (bAvailable == FALSE) return 0;
        OpenClipboard(hwnd);
            //obtinere handle la memoria clipboard
        hClipMemory = GetClipboardData(CF_TEXT);
            //alocare dinamica de memorie (buffer)
        pMyCopy = (char*)malloc(GlobalSize(hClipMemory));
            //obtinere pointer la memoria clipboard
        pClipMemory = (char*)GlobalLock(hClipMemory);
        strcpy(pMyCopy, pClipMemory); //copiere date
        GlobalUnlock(hClipMemory);
        CloseClipboard();
        strcpy(szBufferPaint, pMyCopy);
        SetDlgItemText(hDlg, IDC_EDIT, pMyCopy);
        return TRUE;
    }
    }
    return FALSE;
}
