#undef UNICODE
#include <windows.h>
#include "resource.h"
#include <stdio.h>

double euro = 4.34, dolar = 3.05, r;
char money_type = 'e';

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
    int cash;
    double RON_final, euro_final, dolar_final;
    HWND hwnd;
    char float_number[10002];
    switch (iMsg) {

    case WM_INITDIALOG:
        // setari initiale	  
        hwnd = GetDlgItem(hDlg, IDC_RADIO_EURO);
        SendMessage(hwnd, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
        sprintf_s(float_number, "%.2f", euro);
        SetDlgItemText(hDlg, IDC_EDIT_EURO, float_number);
        sprintf_s(float_number, "%.2f", dolar);
        SetDlgItemText(hDlg, IDC_EDIT_DOLAR, float_number);

        SetDlgItemInt(hDlg, IDC_EDIT_SUMA, 0, FALSE);
        SetDlgItemInt(hDlg, IDC_EDIT_RON_FINAL, 0, FALSE);
        SetDlgItemInt(hDlg, IDC_EDIT_EURO_FINAL, 0, FALSE);
        SetDlgItemInt(hDlg, IDC_EDIT_DOLAR_FINAL, 0, FALSE);
        return TRUE;

    case WM_CLOSE:
        dlgActive = FALSE;
        EndDialog(hDlg, 0);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BUTTON_SCHIMB_CURS:
            r = (double)rand() / RAND_MAX;
            euro = 3.01 + r * (5.51 - 3.01);
            sprintf_s(float_number, "%.2f", euro);
            SetDlgItemText(hDlg, IDC_EDIT_EURO, float_number);

            r = (double)rand() / RAND_MAX;
            dolar = 3 + r * (euro - 3.01);
            sprintf_s(float_number, "%.2f", dolar);
            SetDlgItemText(hDlg, IDC_EDIT_DOLAR, float_number);
            return TRUE;
        case IDC_RADIO_RON:
            money_type = 'r';
            return TRUE;
        case IDC_RADIO_EURO:
            money_type = 'e';
            return TRUE;
        case IDC_RADIO_DOLAR:
            money_type = 'd';
            return TRUE;
        case IDC_BUTTON_CONVERT:
            cash = GetDlgItemInt(hDlg, IDC_EDIT_SUMA, FALSE, FALSE);
            switch (money_type)
            {
            case'r':
                euro_final = cash / euro + 0.1;
                dolar_final = cash / dolar + 0.1;
                SetDlgItemInt(hDlg, IDC_EDIT_RON_FINAL, cash, FALSE);
                SetDlgItemInt(hDlg, IDC_EDIT_EURO_FINAL, euro_final, FALSE);
                SetDlgItemInt(hDlg, IDC_EDIT_DOLAR_FINAL, dolar_final, FALSE);
                break;
            case'e':
                RON_final = cash * euro;
                dolar_final = RON_final / dolar + 0.1;
                SetDlgItemInt(hDlg, IDC_EDIT_EURO_FINAL, cash, FALSE);
                SetDlgItemInt(hDlg, IDC_EDIT_RON_FINAL, RON_final, FALSE);
                SetDlgItemInt(hDlg, IDC_EDIT_DOLAR_FINAL, dolar_final, FALSE);
                break;
            case'd':
                RON_final = cash * dolar;
                euro_final = RON_final / euro + 0.1;
                SetDlgItemInt(hDlg, IDC_EDIT_DOLAR_FINAL, cash, FALSE);
                SetDlgItemInt(hDlg, IDC_EDIT_RON_FINAL, RON_final, FALSE);
                SetDlgItemInt(hDlg, IDC_EDIT_EURO_FINAL, euro_final, FALSE);
                break;
            }
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