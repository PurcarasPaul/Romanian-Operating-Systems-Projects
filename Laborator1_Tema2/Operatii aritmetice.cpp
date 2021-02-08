#undef UNICODE
#include <windows.h>
#include "resource.h"
#include <stdio.h>

bool number1_generated = FALSE, number2_generated = FALSE;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg,
    WPARAM wParam, LPARAM lParam);

BOOL dlgActive = FALSE; //variabila pentru caseta de dialog
HWND hwndMain; //variabila handle a ferestrei principale

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    static char szAppName[] = "Operatii aritmetice";
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
    int random_number,number1,number2,result;
    char result_text[10002];
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
        case IDC_BUTTON_NR1:
            random_number = rand() % 101;
            SetDlgItemInt(hDlg, IDC_EDIT_NR1, random_number, FALSE);
            number1_generated = TRUE;
            return TRUE;
        case IDC_BUTTON_NR2:
            random_number = rand() % 101;
            SetDlgItemInt(hDlg, IDC_EDIT_NR2, random_number, FALSE);
            number2_generated = TRUE;
            return TRUE;
        case IDC_BUTTON_PLUS:
            if (number1_generated == FALSE||number2_generated==FALSE)
                MessageBox(hDlg, "Nu s-au generat ambele numere", "Adunare", MB_OK | MB_ICONSTOP);
            else
            {
                number1 = GetDlgItemInt(hDlg, IDC_EDIT_NR1, FALSE, FALSE);
                number2 = GetDlgItemInt(hDlg, IDC_EDIT_NR2, FALSE, FALSE);
                result = number1 + number2;
                sprintf_s(result_text, "%d + %d = %d", number1,number2,result);
                MessageBox(hDlg, result_text, "Adunare", MB_OK);
            }
            return TRUE;
        case IDC_BUTTON_MINUS:
            if (number1_generated == FALSE || number2_generated == FALSE)
                MessageBox(hDlg, "Nu s-au generat ambele numere", "Scadere", MB_OK | MB_ICONSTOP);
            else
            {
                number1 = GetDlgItemInt(hDlg, IDC_EDIT_NR1, FALSE, FALSE);
                number2 = GetDlgItemInt(hDlg, IDC_EDIT_NR2, FALSE, FALSE);
                result = number1 - number2;
                sprintf_s(result_text, "%d - %d = %d", number1, number2, result);
                MessageBox(hDlg, result_text, "Scadere", MB_OK);
            }
            return TRUE;
        case IDC_BUTTON_MULTIPLY:
            if (number1_generated == FALSE || number2_generated == FALSE)
                MessageBox(hDlg, "Nu s-au generat ambele numere", "Inmultire", MB_OK | MB_ICONSTOP);
            else
            {
                number1 = GetDlgItemInt(hDlg, IDC_EDIT_NR1, FALSE, FALSE);
                number2 = GetDlgItemInt(hDlg, IDC_EDIT_NR2, FALSE, FALSE);
                result = number1 * number2;
                sprintf_s(result_text, "%d * %d = %d", number1, number2, result);
                MessageBox(hDlg, result_text, "Inmultire", MB_OK);
            }
            return TRUE;
        case IDC_BUTTON_DIVIDE:
            if (number1_generated == FALSE || number2_generated == FALSE)
                MessageBox(hDlg, "Nu s-au generat ambele numere", "Impartire", MB_OK | MB_ICONSTOP);
            else
            {
                number1 = GetDlgItemInt(hDlg, IDC_EDIT_NR1, FALSE, FALSE);
                number2 = GetDlgItemInt(hDlg, IDC_EDIT_NR2, FALSE, FALSE);
                if (number2 == 0)
                {
                    MessageBox(hDlg, "Impartire la 0", "Impartire", MB_OK | MB_ICONSTOP);
                    break;
                }
                float(result) = float(number1) / float(number2);
                sprintf_s(result_text, "%d / %d = %f", number1, number2, result);
                MessageBox(hDlg, result_text, "Impartire", MB_OK);
            }
            return TRUE;
        case ID_CANCEL: // operatii ce se executa la actiunea apasarii but. CANCEL
            dlgActive = FALSE;
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}
