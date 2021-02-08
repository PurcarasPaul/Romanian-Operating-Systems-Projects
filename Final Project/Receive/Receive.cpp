#undef UNICODE
#include <windows.h>
#include "resource.h"
#include <iostream>
#include <map>
#include <string>
using namespace std;

map< char, string > ascii_to_binar =
{
{'A',"10111"},
{'B',"111010101"},
{'C',"11101011101"},
{'D',"1110101"},
{'E',"1"},
{'F',"101011101"},
{'G',"111011101"},
{'H',"1010101"},
{'I',"101"},
{'J',"1011101110111"},
{'K',"111010111"},
{'L',"101110101"},
{'M',"1110111"},
{'N',"11101"},
{'O',"11101110111"},
{'P',"10111011101"},
{'Q',"1110111010111"},
{'R',"1011101"},
{'S',"10101"},
{'T',"111"},
{'U',"1010111"},
{'V',"101010111"},
{'W',"101110111"},
{'X',"11101010111"},
{'Y',"1110101110111"},
{'Z',"11101110101"},
};

#define MAXLEN 100
#define WM_MESSAGE_RECEIVED WM_APP + 0x100
HWND hwndReceive, hwnd, hDialog;
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
    static char szAppName[] = "MorseReceive";
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
        "MorseReceiveDialog",    // text pentru bara de titlu a ferestrei
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

bool is_number(string& temp_word)
{
    string::const_iterator it = temp_word.begin();
    while (it != temp_word.end() && isdigit(*it))
        ++it;
    return !temp_word.empty() && it == temp_word.end();
}

bool is_letter(string temp_word)
{
    for (int i = 0;i < temp_word.length();i++)
    {
        if (temp_word[i] >='A' && temp_word[i] <= 'Z' || temp_word[i] == ' ')
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

void convert_to_uppercase(string& temp_word)
{
    for (int i = 0;i < temp_word.length();i++)
        if (temp_word[i] >= 'a' && temp_word[i] <= 'z')
            temp_word[i] = temp_word[i] - 32;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;
    switch (iMsg)
    {
    case WM_CREATE: // operatiile ce se executa la crearea ferestrei
        WM_GETFROMCLIP = RegisterWindowMessage(GetFromClip);
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        if (!dlgActive)
        {
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), hwnd, DlgProc);
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
        SendMessage(hDialog, WM_MESSAGE_RECEIVED, 0, 0);
        return 0;
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}


BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    HGLOBAL hGlobalMemory;
    static char szBufferSend[MAXLEN];
    char* pGlobalMemory, * pMyCopy, * pClipMemory;
    BOOL bAvailable;
    HANDLE hClipMemory;
    int nr, factorial = 1, verif = 0;
    switch (iMsg) {
    case WM_INITDIALOG:
        hDialog = hDlg;
        WM_GETFROMCLIP = RegisterWindowMessage(GetFromClip);
        return TRUE;
    case WM_CLOSE:
        dlgActive = FALSE;
        EndDialog(hDlg, 0);
        return TRUE;
    case WM_MESSAGE_RECEIVED:
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

        SetDlgItemText(hDlg, IDC_EDIT, pMyCopy);

        string word, ascii;
        word = pMyCopy;

        convert_to_uppercase(word);
        if (is_letter(word))
        {
            for (char& c : word)
            {
                if (c == ' ')
                    ascii += "00";
                else
                    ascii += ascii_to_binar[c] + "000";
            }
            ascii = ascii.substr(0, ascii.size() - 3);
            strcpy(szBufferSend, ascii.c_str());
        }
        else strcpy(szBufferSend, "Nu ati introdus litere din alfabetul englez.\n");

        HWND hwndReceive = FindWindow("MorseSend", "MorseSendDialog");
        if (hwndReceive != NULL)
        {
            hGlobalMemory = GlobalAlloc(GHND, strlen(szBufferSend) + 1); //alocare memorie in heap
            pGlobalMemory = (char*)GlobalLock(hGlobalMemory); //obtinerea unui pointer la memoria
            for (UINT i = 0;i < strlen(szBufferSend);i++) //copiere date in heap
                *(pGlobalMemory++) = szBufferSend[i];
            GlobalUnlock(hGlobalMemory);
            OpenClipboard(hDlg); //deschidere clipboard
            SetClipboardData(CF_TEXT, hGlobalMemory);//transmitere date
            CloseClipboard();//inchidere clipboard
            SendMessage(hwndReceive, WM_GETFROMCLIP, 0, 0); //notificare receive pt. preluare date
        }
        else
            MessageBox(hDlg, "Nu am gasit aplicatia send!", "Eroare", MB_OK);
        return 0;
    }
    }
    return FALSE;
}