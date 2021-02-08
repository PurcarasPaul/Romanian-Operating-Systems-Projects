#undef UNICODE
#include <windows.h>
#include "resource.h"
#include <process.h>

//Am definit o structura pentru thread
typedef struct
{
    HWND hwnd;
    HANDLE hEvent;
    int contor;
    BOOL bStop;
    BOOL bPornit;
}PARAMS, *PPARAMS;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg,
    WPARAM wParam, LPARAM lParam);

BOOL dlgActive = FALSE; //variabila pentru caseta de dialog
HWND hwndMain; //variabila handle a ferestrei principale

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    static char szAppName[] = "Dialog";
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
        "Un program simplu",    // text pentru bara de titlu a ferestrei
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

//Functia pentru firul de executie folosit pentru incrementarea controlului
void Thread1(PVOID pvoid)
{
    PPARAMS pparams = (PPARAMS)pvoid;
    while (TRUE)
    {
        if (pparams->bStop == TRUE)
            break;
        if (pparams->bPornit == TRUE)
            SetEvent(pparams->hEvent);
        WaitForSingleObject(pparams->hEvent, INFINITE);
        if (pparams->contor < 0)
            pparams->contor = 0;
        Sleep(500);
        SetDlgItemInt(pparams->hwnd, IDC_EDIT, pparams->contor++,FALSE);
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
    //Initializari
    static PARAMS params;
    HFONT hFont;
    HWND hEditCtrl;
    switch (iMsg) {
    case WM_INITDIALOG:
        //Crearea si setarea fontului in casuta IDC_EDIT
        hFont = CreateFont(80, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
            VARIABLE_PITCH, TEXT("Times New Roman"));
        hEditCtrl = GetDlgItem(hDlg, IDC_EDIT);
        SendMessage(hEditCtrl, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
        SetDlgItemInt(hDlg, IDC_EDIT, params.contor,FALSE);

        //Setari initiale pentru thread
        params.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        params.hwnd = hDlg;
        params.bPornit = FALSE;
        params.bStop = FALSE;

        //Dezactivare buton oprire
        EnableWindow(GetDlgItem(hDlg, ID_STOP), FALSE);
        //Se creeaza firul de executie
        _beginthread(Thread1, 0, &params);

        return TRUE;
    case WM_CLOSE:
        params.bPornit = FALSE;
        params.bStop = TRUE;
        SetEvent(params.hEvent);
        dlgActive = FALSE;
        EndDialog(hDlg, 0);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_START:
            //Se dezactiveaza butonul de pornire si se va activa butonul de oprire
            //Se seteaza startea evenimentului la valoarea semnalat
            params.bPornit = TRUE;
            EnableWindow(GetDlgItem(hDlg, ID_START), FALSE);
            EnableWindow(GetDlgItem(hDlg, ID_STOP), TRUE);
            SetEvent(params.hEvent);
            return TRUE;
        case ID_STOP:
            //Se dezactiveaza butonul de oprire si se va activa butonul de pornire
            //Se seteaza startea evenimentului la valoarea nesemnalat
            EnableWindow(GetDlgItem(hDlg, ID_START), TRUE);
            EnableWindow(GetDlgItem(hDlg, ID_STOP), FALSE);
            params.bPornit = false;
            ResetEvent(params.hEvent);
            return TRUE;
        case ID_CANCEL:
            params.bPornit = FALSE;
            params.bStop = TRUE;
            SetEvent(params.hEvent);
            dlgActive = FALSE;
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}