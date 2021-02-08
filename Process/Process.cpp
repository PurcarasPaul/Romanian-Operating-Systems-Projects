#undef UNICODE
#include <windows.h>
#include <stdio.h>

int main() {

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    DWORD dwWaitForChild;
    int iRasp;


    BOOL b = CreateProcess("C:\\Program Files (x86)\\Microsoft Office\\Office12\\winword.exe",
        NULL, NULL, NULL, FALSE, 0, NULL, NULL,
        &si, &pi);

    if (b) {
        // Asteapta ca procesul sa-si inceapa executia 

        dwWaitForChild = WaitForInputIdle(pi.hProcess, 2000);
        switch (dwWaitForChild) {
        case 0:
            printf("Procesul fiu este ready!\n");
            break;
        case WAIT_TIMEOUT:
            printf("Au trecut 2 sec. si procesul fiu nu este ready!\n");
            break;
        case 0xFFFFFFFF:
            printf("Eroare!\n");
            break;
        }

        iRasp = MessageBox(NULL, "Terminam procesul fiu?", "Atentie!", MB_YESNO);
        if (iRasp == IDYES) {
            if (TerminateProcess(pi.hProcess, 2)) {

                DWORD dwP;
                //obtine codul de retur al procesului fiu
                GetExitCodeProcess(pi.hProcess, &dwP);
                printf("Codul de terminare al procesului fiu: %d\n", dwP);
                //eliberare resurse sistem
                CloseHandle(pi.hProcess);
                printf("\nProcesul fiu a fost terminat cu succes\n");
            }
            else {
                //tiparim mesajul de eroare 
                TCHAR buffer[80];
                LPVOID lpMsgBuf;
                DWORD dw = GetLastError();

                FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    dw,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR)&lpMsgBuf,
                    0, NULL);

                wsprintf(buffer, "TerminateProcess() a esuat cu eroarea %d: %s",
                    dw, lpMsgBuf);

                MessageBox(NULL, buffer, "Eroare!", MB_OK);

                LocalFree(lpMsgBuf);
            }
        } // rasp YES
        else
        {
            switch (WaitForSingleObject(pi.hProcess, 10000))
            {
            case WAIT_OBJECT_0:
                printf("Procesul fiu s-a terminat cu succes\n");
                MessageBox(NULL, "Procesul fiu s-a terminat cu succes", "Succes", MB_OK);
                break;
            case WAIT_TIMEOUT:
                MessageBox(NULL, "Timpul s-a expirat (10 sec) si procesul fiu nu s-a terminat", "Timeout", MB_OK);
                printf("Timpul s-a expirat (10 sec) si procesul fiu nu s-a terminat\n");
                if (TerminateProcess(pi.hProcess, 2)) 
                    CloseHandle(pi.hProcess);
                break;
            case WAIT_FAILED: 
                printf("Eroare!\n");
                break;
            }
        } // rasp NO
    }
    else
        printf("Eroare la crearea procesului fiu!\n");

    return 0;
}