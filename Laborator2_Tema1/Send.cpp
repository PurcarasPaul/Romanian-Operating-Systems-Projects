#undef UNICODE
#include <windows.h>

#define MAXLEN 100 

const int ID_BUTTON = 1; // Buton Ok
const int ID_EDIT = 2; // Zona de editare
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    static char szAppName[] = "Send";
    HWND        hwnd;
    MSG         msg;
    WNDCLASSEX  wndclass; // obiectul instantiat din structura WNDCLASSEX

    wndclass.cbSize = sizeof(wndclass);//Dimensiunea structurii
    wndclass.style = CS_HREDRAW | CS_VREDRAW; //Stilul ferestrei
    wndclass.lpfnWndProc = WndProc; // Procedura de fereastra care trateaza mesajele
    // Doua câmpuri care permit rezervarea de spatiu suplimentar in structura class, respectiv structura window.
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance; //Identificatorul instantei ferestrei
    //Stabileste o pictograma pentru toate ferestrele create pe baza clasei window
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    //Stabileste o pictograma pentru cursor
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    //Specifica culoarea de fundal a ferestrei
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL; // Specifica meniul feresterei
    wndclass.lpszClassName = szAppName; // Specifica numele feresterei
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    //Inregistrarea clasei de fereastra
    RegisterClassEx(&wndclass);

    // Crearea ferestrei pe baza clasei de fereastra
    hwnd = CreateWindow(szAppName,         // window class name
        "Comunicare intre aplicatii - SendMessage",     // window caption
        WS_OVERLAPPEDWINDOW,     // window style
        CW_USEDEFAULT,           // initial x position
        CW_USEDEFAULT,           // initial y position
        600,           // initial x size
        400,           // initial y size
        NULL,                    // parent window handle
        NULL,                    // window menu handle
        hInstance,               // program instance handle
        NULL);		             // creation parameters

    ShowWindow(hwnd, iCmdShow); // afiseaza fereastra pe ecran
    //transmite catre WndProc un mesaj de tip WM_PAINT, care are ca rezultat
    //redesenarea ferestrei, adica reactualizarea zonei client a ferestrei
    UpdateWindow(hwnd); // 

    // preia si distribuie mesaje pana se primeste WM_QUIT
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg); // traduce mesajul
        DispatchMessage(&msg); // transmite mesajul catre procedura de fereastra
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    TEXTMETRIC tm;
    HWND hwndReceive; //identificator fereastra receptie
    static int cxChar, cyChar; // dimensiune caractere
    //identificatori pentru fereastra child (controale)
    static HWND hEdit, hButtonOk;
    static char szBuffer[MAXLEN]; //buffer de date
    COPYDATASTRUCT cs; //structura pentru transmiterea datelor
    HDC hdc; //contextul grafic
    PAINTSTRUCT ps; // o variabila la structura de desenare
    RECT rect; //obiect dreptunghi

    switch (iMsg)
    {
    case WM_CREATE: //operatii ce se executa la crearea ferestrei
        hdc = GetDC(hwnd); //obtinerea unui context grafic
        //selectare font curent (font sistem)
        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
        GetTextMetrics(hdc, &tm); //obtinerea caracteristicilor fontului
        cxChar = tm.tmAveCharWidth; // latimea medie a caracterelor
        //spatiu necesar pentru afisarea fontului pe verticala
        cyChar = tm.tmHeight + tm.tmExternalLeading;
        ReleaseDC(hwnd, hdc); //eliberare context grafic

        // Crearea controalelor grafice pe baza clasei de fereastra
        hButtonOk = CreateWindow("button", "OK", WS_CHILD | WS_VISIBLE
            | BS_DEFPUSHBUTTON, 23 * cxChar, 16 * cyChar,
            25 * cxChar, 2 * cyChar, hwnd, (HMENU)ID_BUTTON,
            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        hEdit = CreateWindow("edit", "", WS_CHILD | WS_VISIBLE
            | ES_LEFT | WS_BORDER, 10 * cxChar, 8 * cyChar,
            50 * cxChar, 2 * cyChar, hwnd, (HMENU)ID_EDIT,
            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        return 0;
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BUTTON)
            //determinare buton care a generat mesajul
        { //returnarea textului din controlul ID_EDIT in szBuffer
            GetDlgItemText(hwnd, ID_EDIT, szBuffer, MAXLEN);
            //determinare handle fereastra receive
            //parametrii fct. FindWindow(nume_clasa_fer, titlu_fer_receive)
            hwndReceive = FindWindow("Receive", "ReceiveMessage");
            if (hwndReceive != NULL)
            {
                cs.cbData = strlen(szBuffer);
                cs.lpData = szBuffer;
                SendMessage(hwndReceive, WM_COPYDATA, (WPARAM)hwnd, (LPARAM)&cs);
                SetDlgItemText(hwnd, ID_EDIT, ""); //resetare text din controlul ID_EDIT
            }
            else
                MessageBox(hwnd, "Nu am gasit partenerul Receive!!", "Erroare", MB_OK);
        }
        return 0;

    case WM_DESTROY://operatii ce se executa la distrugerea ferestrei
        PostQuitMessage(0); // insereaza un mesaj de incheiere in coada de mesaje
        return 0;
    }
    // Alte mesaje sunt trimise catre functia implicita de tratare
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
