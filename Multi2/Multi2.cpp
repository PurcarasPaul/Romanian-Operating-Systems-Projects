/*---------------------------------------
   MULTI2.C -- Multitasking Demo
  ---------------------------------------*/
#undef UNICODE
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <process.h>

HANDLE hThread1, hThread2;
DWORD WINAPI Thread1(PVOID);
DWORD WINAPI Thread2(PVOID);
DWORD id[2];

typedef struct
{
    HWND hwnd;  // handle al ferestrei de actualizat
    int  cxClient; //dimensiunea ferestrei pe orizontala
    int  cyClient; //dimensiunea ferestrei pe verticala
    int  cyChar; //dimensiunea caracterului fontului
    BOOL bKill; //flag de terminare a firului de executie
}
PARAMS, * PPARAMS;

LRESULT APIENTRY WndProc(HWND, UINT, WPARAM, LPARAM);

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

//folosim functia CheckBottom pentru testarea atingerii inferioare a ferestrei
int CheckBottom(HWND hwnd, int cyClient, int cyChar, int iLine)
{
    if (iLine * cyChar + cyChar > cyClient)
    {
        InvalidateRect(hwnd, NULL, TRUE);  // produce marcarea interna a unei zone 
                       //rectangulare din suprafata ferestrei hwnd in vederea unei eventuale redesenari
        UpdateWindow(hwnd);//apeleaza WM_PAINT si actualizeaza fereastra
        iLine = 0; //incepem desenarea pe lina 0
    }
    return iLine;  // linia pe care se va face afiºarea 

}

// Window 1: Display increasing sequence of numbers
// ------------------------------------------------

DWORD WINAPI Thread1(PVOID pvoid) //functia primului fir de executie
{
    int     iNum = 0, iLine = 0;
    char    szBuffer[16];
    HDC     hdc;
    PPARAMS pparams; // structura pentru transmitere informaþii

    //Thread1 are acces la structura pasatã de _beginthread prin parametrul pvoid
    //este necesarã conversia explicitã
    pparams = (PPARAMS)pvoid;

    while (!pparams->bKill) //atat timp cat pparams->bKill==FALSE
    {
        if (iNum < 0) //daca iNum negativ, initializam cu 0
            iNum = 0;
        //apelam functia care permite scrierea textului in cadrul ferestrei pe linie noua
        iLine = CheckBottom(pparams->hwnd, pparams->cyClient,
            pparams->cyChar, iLine);
        //suspendare 200 de milisecund
        Sleep(200);

        //in szBuffer retin numarul iNum 
        wsprintf(szBuffer, "%d", iNum += 2);
        //contextul dispozitiv este o structura asociata unui dispozitiv de afisare 
        //aflarea contextului de dispozitiv
        hdc = GetDC(pparams->hwnd);
        //in cazul in care dorim sa scriem pe ecran folosind un anumit format utilizam TextOut
        TextOut(hdc, 0, iLine * pparams->cyChar, szBuffer, strlen(szBuffer));
        //eliberez obiectul contextul grafic
        ReleaseDC(pparams->hwnd, hdc);
        //trecerea la linia urmatoare
        iLine++;
    }
    return 0;
}

//procedura de fereastra a ferestrei din stanga
LRESULT APIENTRY WndProc1(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    static PARAMS params;
    switch (iMsg)
    {
    case WM_CREATE:
        params.hwnd = hwnd; //transmit firului de executie handle-ul ferestrei fiu
        params.cyChar = HIWORD(GetDialogBaseUnits());//transmit firului de execuþie 
                    //dimensiunea verticala a caracterelor fontului sistem (GetDialogBaseUnits())
        hThread1 = CreateThread(NULL, 0, Thread1, &params, 0, &id[0]);//creez firul de executie
        WaitForSingleObject(Thread1, INFINITE);
        CloseHandle(Thread1);
        return 0;

    case WM_SIZE:
        params.cyClient = HIWORD(lParam); //cod folosit pt ca fct firului de executie sa aiba acces la dimensiunea 
                         //verticala a spatiului de afisare (fiu stânga)
        return 0;

    case WM_DESTROY:
        params.bKill = TRUE; //ieºirea din ciclu while din interiorul fct Thread1
        return 0;
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

// Window 2: Display circles of random radii
// -----------------------------------------

DWORD WINAPI Thread2(PVOID pvoid)
{
    HDC     hdc;
    int     iDiameter;
    PPARAMS pparams;
    HBRUSH hBrush;
    int   iRed, iGreen, iBlue;

    pparams = (PPARAMS)pvoid;

    while (!pparams->bKill) //pparams->bKill==FALSE
    {
        InvalidateRect(pparams->hwnd, NULL, TRUE); //redesenare
        UpdateWindow(pparams->hwnd);//actualizare fereastra

        iDiameter = rand() % (max(1, min(pparams->cxClient, pparams->cyClient)));
        //lungimea diametrului cercului este aleatoare in cadrul ferestrei
        //aceasta nu depaseste marginile ferestrei

        hdc = GetDC(pparams->hwnd); //aflarea contextului de dispozitiv

        //umplerea figurilor geometrice cu diferite culori
        iRed = rand() & 255;
        iGreen = rand() & 255;
        iBlue = rand() & 255;

        hBrush = CreateSolidBrush(RGB(iRed, iGreen, iBlue));
        SelectObject(hdc, hBrush); //selectezobiectul
        //crearea elipsei
        Rectangle(hdc, (pparams->cxClient - iDiameter) / 2,
            (pparams->cyClient - iDiameter) / 2,
            (pparams->cxClient + iDiameter) / 2,
            (pparams->cyClient + iDiameter) / 2);
        //eliberarea continutului grafic 
        ReleaseDC(pparams->hwnd, hdc);
        Sleep(500);//suspendare 500 de milisecunde

    }
    return 0;
}

//procedura de fereastra pentru fereastra dreapta
LRESULT APIENTRY WndProc2(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    static PARAMS params;

    switch (iMsg)
    {
    case WM_CREATE:
        params.hwnd = hwnd; //transmit firului de executie handle-ul ferestrei fiu
        params.cyChar = HIWORD(GetDialogBaseUnits());//transmit firului de execuþie 
                    //dimensiunea verticala a caracterelor fontului sistem (GetDialogBaseUnits())
        hThread2 = CreateThread(NULL, 0, Thread2, &params, 0, &id[1]);//creez firul de executie
        WaitForSingleObject(Thread2, INFINITE);
        CloseHandle(Thread2);
        return 0;

    case WM_SIZE:
        params.cxClient = LOWORD(lParam);  //cod folosit pt ca fct firului de executie sa aiba acces la dimensiunea 
                         //orizontala a spatiului de afisare (fiu dreapta)
        params.cyClient = HIWORD(lParam);
        //cod folosit pt ca fct firului de executie sa aiba acces la dimensiunea 
                        //verticala a spatiului de afisare (fiu dreapta)
        return 0;

    case WM_DESTROY:
        params.bKill = TRUE;
        return 0;
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

// Main window to create child windows
// -----------------------------------

//procedura de fereastra pentru fereastra principala
LRESULT APIENTRY WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    const static char* szChildClass[] = { "Child1", "Child2" };
    static HWND    hwndChild[2];
    static WNDPROC ChildProc[] = { WndProc1, WndProc2 };
    HINSTANCE      hInstance;
    int            i, cxClient, cyClient;
    WNDCLASSEX     wndclass;

    switch (iMsg)
    {
    case WM_CREATE:
        hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
        // determinarea valorii variabilei hInstance cu lungimea ferestrei
        wndclass.cbSize = sizeof(wndclass);
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = NULL;
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wndclass.lpszMenuName = NULL;
        wndclass.hIconSm = NULL;

        for (i = 0; i < 2; i++)
        {
            wndclass.lpfnWndProc = ChildProc[i]; //cele doua ferestre fiu
            wndclass.lpszClassName = szChildClass[i]; //clasele ferestrelor fiu

            RegisterClassEx(&wndclass); // inregistrarea clasei de fereastra
            //crearea celor doua ferestre in cadrul ferestrei principale
            hwndChild[i] = CreateWindow(szChildClass[i], NULL,
                WS_CHILDWINDOW | WS_BORDER | WS_VISIBLE,
                0, 0, 0, 0, hwnd, (HMENU)i, hInstance, NULL);
        }

        return 0;

    case WM_SIZE:

        cxClient = LOWORD(lParam); //dimensiunea ferestrei pe orizontala
        //Codul de notificare este trimis ca HIWORD(wParam), cealalta jumatate a mesajului 
        //care ne-a dat anterior numarul de identificare al controlului (LOWORD(wParam)).  
        cyClient = HIWORD(lParam); //dimensiunea ferestrei pe verticala

        //plasarea celor doua ferestre fiu în cadrul ferestrei parinte
        for (i = 0; i < 2; i++)
            MoveWindow(hwndChild[i], (i % 2) * cxClient / 2, 0,
                cxClient / 2, cyClient, TRUE);
        return 0;

    case WM_CHAR:
        if (wParam == '\x1B') //distrugerea ferestrei la apasarea tastei ESC
            DestroyWindow(hwnd);

        return 0;

    case WM_DESTROY:
        PostQuitMessage(0); // trimiterea unui mesaj de iesire (terminare)
        return 0;
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}