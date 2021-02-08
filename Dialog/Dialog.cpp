#undef UNICODE
#include <windows.h>
#include "resource.h"

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

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;
    switch (iMsg)
    {
    case WM_CREATE: // operatiile ce se executa la crearea ferestrei
       // se creaza caseta de dialog
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

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////


BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    char text[100];
    static char buff[10002] = "\0"; //Am initializat o noua variabila de tip char
    switch (iMsg) {

    case WM_INITDIALOG:
        // setari initiale	     
        return TRUE;

    case WM_CLOSE:
        dlgActive = FALSE;
        EndDialog(hDlg, 0);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        /* Am eliminat urmatoarele functii din ID_OK:
            GetDlgItemText(hDlg, IDC_EDIT, text, 100);
            MessageBox(hDlg, text, "Ati selectat OK", MB_OK); 
            EndDialog (hDlg, 0) 
           Si am adaugat urmatoarele functii:
            strcat_s(buff, "HelloWin! \r\n");
            SetDlgItemText(hDlg, IDC_EDIT, buff);
        */
        case ID_OK: //operatii ce se executa la actiunea apasarii butonului OK 
            strcat_s(buff, "HelloWin! \r\n");       
            SetDlgItemText(hDlg, IDC_EDIT, buff);
            dlgActive = FALSE;
            return TRUE;
        case ID_CANCEL: // operatii ce se executa la actiunea apasarii but. CANCEL
            MessageBox(hDlg, "Abandon!", "Ati selectat CANCEL", MB_OK);
            dlgActive = FALSE;
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

