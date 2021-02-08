#undef UNICODE
#include <windows.h>
#include "resource.h"
//Am adaugat libraria stdio.h
#include <stdio.h>

//Variabila pentru a putea verifica daca s-a afisat un nr aleator sau nu
bool number_generated = FALSE;

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
    //variabile int pentru stocarea numerelor,long long pentru factorial deoarece sunt numere mari si char pentru transformarea din long long in string
    int random_number, number;
    long long factorial=1;
    char  buffer[10002];
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
        /*case ID_RANDOM este folosit pentru afisarea unui numar aleator in casuta IDC_EDIT_NR
        * In momentul in care este apasat butonul pentru afisarea unui numar aleator,number_generated va fi setat TRUE din FALSE
        * pentru a putea verifica atunci cand se apasa butonul "Calculeaza factorialul nr aleatoriu generat" daca exista sau nu un numar */
        case ID_RANDOM:
            random_number = rand() % 16;
            SetDlgItemInt(hDlg, IDC_EDIT_NR, random_number,FALSE);
            number_generated = TRUE;
            return TRUE;
        /* case ID_CALC este folosit pentru calcularea factorialului si afisarea lui in casuta IDC_EDIT_REZ
        *  daca nu exista nici un numar in casuta IDC_EDIT_NR,se va afisa o eroare,altfel se va verifica
        *  daca numarul este 0,in acest caz casuta IDC_EDIT_REZ va avea valoare 1,altfel se va calcula factorialul
        *  numarului din casuta IDC_EDIT_NR in variabila long long factorial si apoi rezultatul va fi transformat
        *  in string pentru a putea fii afisat in mod corect */
        case ID_CALC:
            number = GetDlgItemInt(hDlg, IDC_EDIT_NR, FALSE ,FALSE);
            if (number_generated==FALSE)
                MessageBox(hDlg, "Nu s-a generat niciun nr.", "Eroare", MB_OK | MB_ICONSTOP);
            else
            {
                if (number == 0)
                    SetDlgItemInt(hDlg, IDC_EDIT_REZ, 1, FALSE);
                else
                {
                    for (int i = 1;i <= number;i++)
                        factorial *= i;
                    sprintf_s(buffer, "%lld", factorial);
                    SetDlgItemText(hDlg, IDC_EDIT_REZ,buffer);
                }
            }
            return TRUE;
            /* in case ID_CANCEL am eliminat mesajul de iesire din program,in momentul in care butonul "IESIRE" este apasat
            *  aplicatia se va inchide instant fara alte mesaje de confirmare */
        case ID_CANCEL: // operatii ce se executa la actiunea apasarii but. CANCEL
            dlgActive = FALSE;
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}
