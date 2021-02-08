#undef UNICODE
#include <windows.h>
#include "resource.h"

//Initializari
#define MAXLEN 100
static UINT WM_GETFROMCLIP;
LPCTSTR GetFromClip = "GetFromClipboard";
HGLOBAL hGlobalMemory;
char* pMyCopy, * pClipMemory,* pGlobalMemory;
HWND hwnd, hDialog;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
BOOL CALLBACK DlgProc (HWND hDlg, UINT iMsg,WPARAM wParam, LPARAM lParam);

BOOL dlgActive=FALSE; //variabila pentru caseta de dialog
HWND hwndMain; //variabila handle a ferestrei principale

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
     {
     static char szAppName[] = "Client" ;
     HWND        hwnd ;
     MSG         msg ;
     WNDCLASSEX  wndclass ;

     wndclass.cbSize        = sizeof (wndclass) ;
     wndclass.style    = CS_HREDRAW | CS_VREDRAW ;  //optiuni pentru stilul clasei 
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
                    // incarca pictograma fisierelor de tip aplicatie
     wndclass.hCursor  = LoadCursor (NULL, IDC_ARROW) ; // incarca cursorul sageata  
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
                       // fondul ferstrei de culoare alba     
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;
     wndclass.hIconSm       = LoadIcon (NULL, IDI_APPLICATION) ;

     RegisterClassEx (&wndclass) ;

     hwnd=CreateWindow(szAppName, //numele clasei inregistrat cu RegisterClass
			"ClientDialog",    // text pentru bara de titlu a ferestrei
			WS_OVERLAPPEDWINDOW,   // stilul ferestrei
			CW_USEDEFAULT,      // pozitia orizontala implicit�
			CW_USEDEFAULT,      // pozitia verticala implicita
			CW_USEDEFAULT,       // latimea implicita
			CW_USEDEFAULT,       // inaltimea implicita
			NULL,               // handle-ul ferestrei parinte
			NULL,               // handle-ul meniului ferestrei
			hInstance,          // proprietara ferestrei 
NULL) ;         


     SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE);
     // schimba dimensiunea, pozitia si ordinea z a ferestrei copil, a ferestrei pop-up
     ShowWindow(hwnd, iCmdShow);
     UpdateWindow(hwnd);
     hwndMain = hwnd;

    
 
 while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
 return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
    {
    static HINSTANCE hInstance ;
    switch (iMsg)
    {
        case WM_CREATE : // operatiile ce se executa la crearea ferestrei
             WM_GETFROMCLIP = RegisterWindowMessage(GetFromClip);
             hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;
             if (!dlgActive) 
             {
		     DialogBox (hInstance, MAKEINTRESOURCE(IDD_DIALOG), 
                        hwnd, DlgProc) ;
             PostMessage(hwnd,WM_CLOSE,0,0); 
                      // insereaza un nou mesaj nou in coada de asteptare
             }
             return 0 ;
        case WM_DESTROY :
               PostQuitMessage (0) ; // insereaza un mesaj de incheiere 
               return 0;
    }
    //In momentul in care se gaseste un mesaj in clipboard,se va trimite un mesaj in casuta hDlg
     if (iMsg == WM_GETFROMCLIP)
     {
         SendMessage(hDialog, WM_GETFROMCLIP, 0, 0);
         return 0;
     }
     return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
     }

BOOL CALLBACK DlgProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    //Initializari
    static char szBufferReceive[MAXLEN], szBuffer[MAXLEN];
    BOOL bAvailable;
    HANDLE hClipMemory;
    switch (iMsg) {
    case WM_INITDIALOG : 
         hDialog = hDlg;
         WM_GETFROMCLIP = RegisterWindowMessage(GetFromClip);
         return TRUE ;
    case WM_CLOSE:
        dlgActive = FALSE;
        EndDialog(hDlg, 0);
        return TRUE;
        break;
    case WM_COMMAND :
        /*In momentul in care se apasa pe butonul "FACTORIAL",acesta va lua mesajul din casuta IDC_EDIT
        * Apoi va cauta casuta ServerDialog din Server,daca se gaseste va copia mesajul din casuta IDC_EDIT
        * in clipboard si va trimite un mesaj catre Server pentru a receptiona textul copiat in clipboard */
        switch (LOWORD(wParam))
        {
        case FACTORIAL:
        {
            GetDlgItemText(hDlg, IDC_EDIT, szBuffer, MAXLEN);
            HWND hwndReceive = FindWindow("Server", "ServerDialog");
            if (hwndReceive != NULL)
            {
                hGlobalMemory = GlobalAlloc(GHND, strlen(szBuffer) + 1); //alocare memorie in heap
                pGlobalMemory = (char*)GlobalLock(hGlobalMemory); //obtinerea unui pointer la memoria
                for (UINT i = 0;i < strlen(szBuffer);i++) //copiere date in heap
                    *(pGlobalMemory++) = szBuffer[i];
                GlobalUnlock(hGlobalMemory);
                OpenClipboard(hDlg); //deschidere clipboard
                SetClipboardData(CF_TEXT, hGlobalMemory);//transmitere date
                CloseClipboard();//inchidere clipboard
                SendMessage(hwndReceive, WM_GETFROMCLIP, 0, 0); //notificare receive pt. preluare date
                /*Secventa aceasta de cod va copia mesajul din clipboard trimis de catre server
            * si va seta in casuta IDC_REZ textul primit */
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
                strcpy(szBufferReceive, pMyCopy);
                SetDlgItemText(hDlg, IDC_REZ, szBufferReceive);
            }
            else MessageBox(hDlg, "Nu am gasit Serverul!", "Eroare", MB_OK);
            return TRUE;
        }
        case ID_CANCEL:
            dlgActive = FALSE;
            EndDialog(hDlg, 0);
            return TRUE;
        }
   }
    return FALSE;
}