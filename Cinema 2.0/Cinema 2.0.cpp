#undef UNICODE
#include <windows.h>
#include <stdio.h>
#include "resource.h"

int NrBileteDisponibile;
HANDLE hThread[1002];
HANDLE semafor_bilet_rezervat;
DWORD idThread[1002];
char text[1000] = "\0";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI VanzareBilete(PVOID n);

BOOL dlgActive = FALSE;
HWND hwndMain;
HWND hDlg;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    static char szAppName[] = "Cinema";
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
        "Vanzare bilete cinema",    // text pentru bara de titlu a ferestrei
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

DWORD WINAPI VanzareBilete(PVOID n)
{
    char temp[1000] = "\0";
    BOOL done = false;
    int id = (int)n;
    int NumarBileteVanduteDeThread = 0;
    while (!done)
    {
        WaitForSingleObject(semafor_bilet_rezervat, INFINITE);
        if (NrBileteDisponibile == 0)
            done = true;
        else
        {
            NumarBileteVanduteDeThread++;
            NrBileteDisponibile--;
            sprintf_s(temp, "Vanzatorul %d a vandut 1 bilet (Au ramas %d bilete) \r\n", id + 1, NrBileteDisponibile);
            strcat_s(text, temp);
        }
        ReleaseSemaphore(semafor_bilet_rezervat, 1, NULL);
    }
    sprintf_s(temp,"Vanzatorul %d a vandut %d bilete! \r\n",id+1,NumarBileteVanduteDeThread);
    strcat_s(text, temp);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;
    switch (iMsg)
    {
    case WM_CREATE: // operatiile ce se executa la crearea ferestrei
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
    int NR_VANZATORI;
    switch (iMsg) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_CLOSE:
        dlgActive = FALSE;
        EndDialog(hDlg, 0);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_SELL:
            memset(text, 0, 1000);
            NR_VANZATORI = GetDlgItemInt(hDlg, IDC_EDIT2, FALSE, FALSE);
            NrBileteDisponibile = GetDlgItemInt(hDlg, IDC_EDIT1, FALSE, FALSE);
            semafor_bilet_rezervat = CreateSemaphore(0, 1, 1, "SemaforBiletRezervat");
            for (int i = 0;i < NR_VANZATORI; i++)
                hThread[i] = CreateThread(NULL, 0, VanzareBilete, (VOID*)i, 0, &idThread[i]);
            WaitForMultipleObjects(NR_VANZATORI, hThread, TRUE, INFINITE);
            SetDlgItemText(hDlg, IDC_EDIT, text);

            for (int i = 0;i < NR_VANZATORI;i++)
                CloseHandle(hThread[i]);
            CloseHandle(semafor_bilet_rezervat);
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
