/*------------------------------------------------------------
   Accesare servicii HTTP
  ------------------------------------------------------------*/

#undef UNICODE
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include <wininet.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL GetPage_http(HWND hDlg, int transferFile);
BOOL GetPage_https(HWND hDlg, int transferFile);
BOOL GetSimplePage_http(HWND hDlg, int transferFile);
BOOL dlgActive = FALSE;
HWND hwndMain;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static char szAppName[] = "Http";
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

	hwnd = CreateWindow(szAppName,         // window class name
		"Acces http/https",            // window caption
		WS_OVERLAPPEDWINDOW,     // window style
		CW_USEDEFAULT,           // initial x position
		CW_USEDEFAULT,           // initial y position
		CW_USEDEFAULT,           // initial x size
		CW_USEDEFAULT,           // initial y size
		NULL,                    // parent window handle
		NULL,                    // window menu handle
		hInstance,               // program instance handle
		NULL);		             // creation parameters

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

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	switch (iMsg)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		if (!dlgActive) {
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), hwnd, DlgProc);
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////



BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd_radio;   int transferFile = 0;
	switch (iMsg) {

	case WM_INITDIALOG:
		dlgActive = TRUE;
		hwnd_radio = GetDlgItem(hDlg, IDC_RADIO1);
		SendMessage(hwnd_radio, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		return TRUE;

	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		case ID_OK:
			if (IsDlgButtonChecked(hDlg, IDC_CHECK1) == BST_CHECKED)
				transferFile = 1;
			else
				transferFile = 0;

			if (IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
			{
				GetPage_http(hDlg, transferFile);
			}
			else
			{
				GetPage_https(hDlg, transferFile);
			}
			return TRUE;

		case ID_GetSimplePage:
			hwnd_radio = GetDlgItem(hDlg, IDC_RADIO1);
			SendMessage(hwnd_radio, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
			hwnd_radio = GetDlgItem(hDlg, IDC_RADIO2);
			SendMessage(hwnd_radio, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
			if (IsDlgButtonChecked(hDlg, IDC_CHECK1) == BST_CHECKED)
				transferFile = 1;
			else
				transferFile = 0;
			GetSimplePage_http(hDlg, transferFile);
			return TRUE;
		case ID_CANCEL:
			dlgActive = FALSE;
			EndDialog(hDlg, 0);
			return TRUE;

		case ID_RESET:
			SetDlgItemText(hDlg, IDC_EDIT_URL, "");
			SetDlgItemText(hDlg, IDC_EDIT_SHOW, "");
			SetDlgItemInt(hDlg, IDC_STATIC_LENGTH_SHOW, 0, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


BOOL GetPage_https(HWND hDlg, int transferFile) {

	HINTERNET hInternet, hHTTPSession, hHTTPRequest;
	HANDLE hFile = NULL; HWND hwnd_check;
	char url[100];
	char* pdest, * server;
	int  count; char msg[100], buffer_edit[200000] = "";
	DWORD dwWritten;

	SetDlgItemText(hDlg, IDC_EDIT_SHOW, "");
	GetDlgItemText(hDlg, IDC_EDIT_URL, url, 100);

	pdest = strstr(url, "https://");
	if (pdest != NULL)
		strcpy_s(url, pdest + 8);

	pdest = strchr(url, '/');
	if (pdest != NULL) {
		count = pdest - url;
		server = new char[count + 1];
		strncpy(server, url, count);
		server[count] = '\0';
		strcpy_s(url, pdest + 1);
	}
	else {
		server = new char[strlen(url) + 1];
		strcpy(server, url);
		url[0] = '\0';
	}

	hInternet = InternetOpen("My C++ App", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL) {
		MessageBox(hDlg, "Eroare dechidere conexiune Internet!",
			"Eroare Internet", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hHTTPSession = InternetConnect(hInternet, server, INTERNET_DEFAULT_HTTPS_PORT,
		NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (hHTTPSession == NULL) {
		MessageBox(hDlg, "Eroare accesare serviciu HTTP!",
			"Eroare HTTP!", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hHTTPRequest = HttpOpenRequest(hHTTPSession, "GET", url,
		HTTP_VERSION, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
	if (hHTTPRequest == NULL) {
		MessageBox(hDlg, "Eroare URL!", "Eroare URL!", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if (!HttpSendRequest(hHTTPRequest, NULL, 0, 0, 0))
	{
		LPVOID lpMsgBuf; char buffer[255];
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		wsprintf(buffer, "HttpSendRequest() a esuat cu eroarea %d: %s", dw, lpMsgBuf);

		MessageBox(NULL, buffer, "Eroare SendRequest!", MB_OK);

		LocalFree(lpMsgBuf);
		return FALSE;
	}

	DWORD dwFileSize = BUFSIZ;

	// alocare buffer pentru receptie date
	char* buffer = new char[dwFileSize + 1];
	int length_file = 0;

	if (transferFile == 1)
		hFile = CreateFile("download", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true) {
		DWORD dwBytesRead;
		BOOL bRead;

		bRead = InternetReadFile(
			hHTTPRequest,
			buffer,
			dwFileSize + 1,
			&dwBytesRead);

		if (dwBytesRead == 0) break;

		if (!bRead) {
			sprintf_s(msg, "InternetReadFile error : <%lu>\n", GetLastError());
			MessageBox(hDlg, msg, "GetLastError()", MB_OK | MB_ICONERROR);
		}
		else {
			buffer[dwBytesRead] = 0;
			length_file = length_file + dwBytesRead;
			SetDlgItemInt(hDlg, IDC_STATIC_LENGTH_SHOW1, length_file, 0);
			Sleep(10);
			if (transferFile == 1)
			{
				WriteFile(hFile, buffer, dwBytesRead, &dwWritten, NULL);
			}
			else
			{
				if (length_file < sizeof(buffer_edit))
					strcat(buffer_edit, buffer);
			}
		}
	}

	if (transferFile == 1)
	{
		CloseHandle(hFile);
		SetDlgItemText(hDlg, IDC_EDIT_SHOW, "Transfer terminat! Fisierul creat: download");
		hwnd_check = GetDlgItem(hDlg, IDC_CHECK1);
		SendMessage(hwnd_check, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
	}
	else
		SetDlgItemText(hDlg, IDC_EDIT_SHOW, buffer_edit);

	SetDlgItemInt(hDlg, IDC_STATIC_LENGTH_SHOW, length_file, 0);


	// inchidere conexiuni
	InternetCloseHandle(hHTTPRequest);
	InternetCloseHandle(hHTTPSession);
	InternetCloseHandle(hInternet);
	return TRUE;
}

BOOL GetPage_http(HWND hDlg, int transferFile) {

	HINTERNET hInternet, hHTTPSession, hHTTPRequest;
	HANDLE hFile = NULL; HWND hwnd_check;
	char url[100];
	char* pdest, * server;
	int  count; char msg[100], buffer_edit[200000] = "";
	DWORD dwWritten;

	SetDlgItemText(hDlg, IDC_EDIT_SHOW, "");
	GetDlgItemText(hDlg, IDC_EDIT_URL, url, 100);

	pdest = strstr(url, "http://");
	if (pdest != NULL)
		strcpy_s(url, pdest + 7);

	pdest = strchr(url, '/');
	if (pdest != NULL) {
		count = pdest - url;
		server = new char[count + 1];
		strncpy(server, url, count);
		server[count] = '\0';
		strcpy_s(url, pdest + 1);
	}
	else {
		server = new char[strlen(url) + 1];
		strcpy(server, url);
		url[0] = '\0';
	}

	hInternet = InternetOpen("My C++ App", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL) {
		MessageBox(hDlg, "Eroare dechidere conexiune Internet!",
			"Eroare Internet", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hHTTPSession = InternetConnect(hInternet, server, INTERNET_DEFAULT_HTTP_PORT,
		NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (hHTTPSession == NULL) {
		MessageBox(hDlg, "Eroare accesare serviciu HTTP!",
			"Eroare HTTP!", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hHTTPRequest = HttpOpenRequest(hHTTPSession, "GET", url,
		HTTP_VERSION, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (hHTTPRequest == NULL) {
		MessageBox(hDlg, "Eroare URL!", "Eroare URL!", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if (!HttpSendRequest(hHTTPRequest, NULL, 0, 0, 0))
	{
		LPVOID lpMsgBuf; char buffer[255];
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		wsprintf(buffer, "HttpSendRequest() a esuat cu eroarea %d: %s", dw, lpMsgBuf);

		MessageBox(NULL, buffer, "Eroare SendRequest!", MB_OK);

		LocalFree(lpMsgBuf);
		return FALSE;
	}

	DWORD dwFileSize = 1000;

	// alocare buffer pentru receptie date
	char* buffer = new char[dwFileSize + 1];
	int length_file = 0;

	if (transferFile == 1)
		hFile = CreateFile("download", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true) {
		DWORD dwBytesRead;
		BOOL bRead;

		bRead = InternetReadFile(
			hHTTPRequest,
			buffer,
			dwFileSize + 1,
			&dwBytesRead);

		if (dwBytesRead == 0) break;

		if (!bRead) {
			sprintf_s(msg, "InternetReadFile error : <%lu>\n", GetLastError());
			MessageBox(hDlg, msg, "GetLastError()", MB_OK | MB_ICONERROR);
		}
		else {
			buffer[dwBytesRead] = 0;
			length_file = length_file + dwBytesRead;
			SetDlgItemInt(hDlg, IDC_STATIC_LENGTH_SHOW1, length_file, 0);
			Sleep(10);
			if (transferFile == 1)
			{
				WriteFile(hFile, buffer, dwBytesRead, &dwWritten, NULL);
			}
			else
				if (length_file < sizeof(buffer_edit))
					strcat(buffer_edit, buffer);
		}
	}

	if (transferFile == 1)
	{
		CloseHandle(hFile);
		SetDlgItemText(hDlg, IDC_EDIT_SHOW, "Transfer terminat! Fisierul creat: download");
		hwnd_check = GetDlgItem(hDlg, IDC_CHECK1);
		SendMessage(hwnd_check, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
	}
	else
		SetDlgItemText(hDlg, IDC_EDIT_SHOW, buffer_edit);

	SetDlgItemInt(hDlg, IDC_STATIC_LENGTH_SHOW, length_file, 0);

	// inchidere conexiuni
	InternetCloseHandle(hHTTPRequest);
	InternetCloseHandle(hHTTPSession);
	InternetCloseHandle(hInternet);
	return TRUE;
}

BOOL GetSimplePage_http(HWND hDlg, int transferFile) 
{
	HINTERNET hInternet, hHTTPSession, hHTTPRequest;
	HANDLE hFile = NULL; HWND hwnd_check;
	char url[100];
	char* pdest, * server;
	int  count; char msg[100], buffer_edit[200000] = "";
	DWORD dwWritten;

	SetDlgItemText(hDlg, IDC_EDIT_SHOW, "");
	GetDlgItemText(hDlg, IDC_EDIT_URL, url, 100);

	pdest = strstr(url, "http://");
	if (pdest != NULL)
		strcpy_s(url, pdest + 7);

	pdest = strchr(url, '/');
	if (pdest != NULL) {
		count = pdest - url;
		server = new char[count + 1];
		strncpy(server, url, count);
		server[count] = '\0';
		strcpy_s(url, pdest + 1);
	}
	else {
		server = new char[strlen(url) + 1];
		strcpy(server, url);
		url[0] = '\0';
	}

	hInternet = InternetOpen("My C++ App", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL) {
		MessageBox(hDlg, "Eroare dechidere conexiune Internet!",
			"Eroare Internet", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hHTTPSession = InternetConnect(hInternet, server, INTERNET_DEFAULT_HTTP_PORT,
		NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (hHTTPSession == NULL) {
		MessageBox(hDlg, "Eroare accesare serviciu HTTP!",
			"Eroare HTTP!", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hHTTPRequest = InternetOpenUrl(hInternet, url,
		NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (hHTTPRequest == NULL) {
		MessageBox(hDlg, "Eroare URL!", "Eroare URL!", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if (!HttpSendRequest(hHTTPRequest, NULL, 0, 0, 0))
	{
		LPVOID lpMsgBuf; char buffer[255];
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		wsprintf(buffer, "HttpSendRequest() a esuat cu eroarea %d: %s", dw, lpMsgBuf);

		MessageBox(NULL, buffer, "Eroare SendRequest!", MB_OK);

		LocalFree(lpMsgBuf);
		return FALSE;
	}

	DWORD dwFileSize = 1000;

	// alocare buffer pentru receptie date
	char* buffer = new char[dwFileSize + 1];
	int length_file = 0;

	if (transferFile == 1)
		hFile = CreateFile("download", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true) {
		DWORD dwBytesRead;
		BOOL bRead;

		bRead = InternetReadFile(
			hHTTPRequest,
			buffer,
			dwFileSize + 1,
			&dwBytesRead);

		if (dwBytesRead == 0) break;

		if (!bRead) {
			sprintf_s(msg, "InternetReadFile error : <%lu>\n", GetLastError());
			MessageBox(hDlg, msg, "GetLastError()", MB_OK | MB_ICONERROR);
		}
		else {
			buffer[dwBytesRead] = 0;
			length_file = length_file + dwBytesRead;
			SetDlgItemInt(hDlg, IDC_STATIC_LENGTH_SHOW1, length_file, 0);
			Sleep(10);
			if (transferFile == 1)
			{
				WriteFile(hFile, buffer, dwBytesRead, &dwWritten, NULL);
			}
			else
				if (length_file < sizeof(buffer_edit))
					strcat(buffer_edit, buffer);
		}
	}

	if (transferFile == 1)
	{
		CloseHandle(hFile);
		SetDlgItemText(hDlg, IDC_EDIT_SHOW, "Transfer terminat! Fisierul creat: download");
		hwnd_check = GetDlgItem(hDlg, IDC_CHECK1);
		SendMessage(hwnd_check, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
	}
	else
		SetDlgItemText(hDlg, IDC_EDIT_SHOW, buffer_edit);

	SetDlgItemInt(hDlg, IDC_STATIC_LENGTH_SHOW, length_file, 0);

	// inchidere conexiuni
	InternetCloseHandle(hHTTPRequest);
	InternetCloseHandle(hHTTPSession);
	InternetCloseHandle(hInternet);
	return TRUE;
}