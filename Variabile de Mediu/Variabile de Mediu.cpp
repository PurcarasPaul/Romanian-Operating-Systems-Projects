#undef UNICODE
#include <windows.h>
#include <stdio.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg,
    WPARAM wParam, LPARAM lParam);

BOOL dlgActive = FALSE; //variabila pentru caseta de dialog
HWND hwndMain; //variabila handle a ferestrei principale

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    static char szAppName[] = "Variabile de mediu";
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
        "VariabileMediu",    // text pentru bara de titlu a ferestrei
        WS_OVERLAPPEDWINDOW,   // stilul ferestrei
        CW_USEDEFAULT,      // pozitia orizontala implicit�
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
    //Initializari
    char delimitator[2] = "=", buffer[2000];
    char* env_win, * NumeVarMediu;
    char VarMed[100], lpBuffer[2000] = "", szBufferList[2000] = "";
    LPTSTR lpszVariable;
    switch (iMsg)
    {
    case WM_INITDIALOG:
        /*Alocare memorie pentru copia blocului variabilelor de mediu
          Copiem variabilele de mediu in buffer din lpszVariable
          In variabila NumeVarMediu se copiaza blocuri din string
          In szBufferList se vor copia toate variabilele de mediu
          care vor fii apoi afisate in casuta IDC_EDIT_LIST*/
        env_win=GetEnvironmentStrings();
        lpszVariable = (LPTSTR)env_win;
        while (strlen(lpszVariable) > 0)
        {
            strcpy_s(buffer, lpszVariable);
            NumeVarMediu = strtok(buffer, delimitator);
            strcat_s(szBufferList, buffer);
            strcat_s(szBufferList, "\r\n");
            lpszVariable += strlen(lpszVariable) + 1;
        }
        //Elibereaza memoria alocata pentru copia blocului variabilelor de mediu
        //Apoi setam in casuta IDC_EDIT_LIST lista variabilelor de mediu stocate in szBufferList
        FreeEnvironmentStrings(env_win);
        SetDlgItemText(hDlg, IDC_EDIT_LIST, szBufferList);
        return TRUE;

    case WM_CLOSE:
        dlgActive = FALSE;
        EndDialog(hDlg, 0);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            /*La apasarea butonului "Valoare Variabila Mediu",se va stoca textul
            din casuta de editare IDC_EDIT in variabila VarMed,in cazul in care
            numele variabilei de mediu exista,se va afisa o casuta cu valoarea acesteia,
            altfel va aparea o eroare*/
        case ID_OK:
            GetDlgItemText(hDlg, IDC_EDIT, VarMed, 100);
            if (GetEnvironmentVariable(VarMed, lpBuffer, 2000))
                MessageBox(hDlg, lpBuffer, "Valoarea variabilei de mediu", MB_OK);
            else MessageBox(hDlg, "Nu s-a gasit variabila de mediu", "EROARE", MB_OK | MB_ICONSTOP);
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