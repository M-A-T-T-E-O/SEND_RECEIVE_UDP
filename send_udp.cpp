#include <stdio.h> 
#include "Ws2tcpip.h"
#include <stdlib.h> 
#include <string.h> 
#include <time.h>
#include <sys/types.h> 
#include <wingdi.h>
#include <winsock2.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string.h>
#include <windows.h>


using namespace std;
#define ID_BUTTON1 0x8801 


#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996)

#define MAXLINE  4 


int main()
{
	// Server side implementation UDP
	int s_s, i;
	int connected = 1, size = 4;
	static COLORREF bkcolor = RGB(0, 180, 0);
	static COLORREF bkcolor_up = RGB(0, 0, 0);
	static WSADATA wsa;
	static unsigned char buffer[4];
	struct sockaddr_in servaddr;
	struct sockaddr_in servaddr_mon;
	PAINTSTRUCT ps;
	RECT rc;
	MSG msg;
	BOOL bpressed;
	INT hlrm;
	INT handle_son;
	HANDLE text;
	int check = 0;
	int ischanging = 1;
	int first;
	ifstream file_read;
	int file_is_open = 0;
	string line;
	string time_between;
	int mode = 0;
	int auto_mode_finish = 0;
	int time_elapsed;
	string folder = "";
	
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		printf("WSAStartup() failed with error code %d", WSAGetLastError());
		connected = 0;
	}

	s_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (s_s == SOCKET_ERROR) {
		printf("sending socket() failed with error code %d", WSAGetLastError());
		connected = 0;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&servaddr_mon, 0, sizeof(servaddr_mon));

	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(4001);
	servaddr_mon.sin_family = AF_INET; // IPv4 
	servaddr_mon.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr_mon.sin_port = htons(3001);

	// Parent window
	HWND main_control = CreateWindow(TEXT("STATIC"), L"CONTROL BOARD", WS_VISIBLE | BS_GROUPBOX,
		0, 0, 840, 160, HWND_DESKTOP, NULL, NULL, NULL);


	// Sons
	HWND wow_on = CreateWindow(TEXT("button"), TEXT("WONW_ON"), WS_VISIBLE | WS_POPUP | BS_AUTORADIOBUTTON,
		30, 30, 120, 30, NULL, NULL, NULL, NULL);
	HWND wow_off = CreateWindow(TEXT("button"), TEXT("WONW_OFF"), WS_VISIBLE | WS_POPUP | BS_AUTORADIOBUTTON,
		30, 65, 120, 30, NULL, NULL, NULL, NULL);
	HWND woffw_on = CreateWindow(TEXT("button"), TEXT("WOFFW_ON"), WS_VISIBLE | WS_POPUP | BS_AUTORADIOBUTTON,
		170, 30, 120, 30, NULL, NULL, NULL, NULL);
	HWND woffw_off = CreateWindow(TEXT("button"), TEXT("WOFFW_OFF"), WS_VISIBLE | WS_POPUP | BS_AUTORADIOBUTTON,
		170, 65, 120, 30, NULL, NULL, NULL, NULL);
	HWND ground_on = CreateWindow(TEXT("button"), TEXT("Ground_ON"), WS_VISIBLE | WS_POPUP | BS_AUTORADIOBUTTON,
		310, 30, 120, 30, NULL, NULL, NULL, NULL);
	HWND ground_off = CreateWindow(TEXT("button"), TEXT("Ground_OFF"), WS_VISIBLE | WS_POPUP | BS_AUTORADIOBUTTON,
		310, 65, 120, 30, NULL, NULL, NULL, NULL);
	HWND flight_on = CreateWindow(TEXT("button"), TEXT("Flight_ON"), WS_VISIBLE | WS_POPUP | BS_AUTORADIOBUTTON,
		450, 30, 120, 30, NULL, NULL, NULL, NULL);
	HWND flight_off = CreateWindow(TEXT("button"), TEXT("Flight_OFF"), WS_VISIBLE | WS_POPUP | BS_AUTORADIOBUTTON,
		450, 65, 120, 30, NULL, NULL, NULL, NULL);


	// others
	HWND send_push = CreateWindow(TEXT("button"), TEXT("SEND"), WS_VISIBLE | WS_POPUP,
		635, 30, 70, 70, NULL, NULL, NULL, NULL);
	HWND auto_mode = CreateWindow(TEXT("button"), TEXT("AUTO"), WS_VISIBLE | WS_POPUP,
		710, 30, 80, 30, NULL, NULL, NULL, NULL);
	HWND manual_mode = CreateWindow(TEXT("button"), TEXT("MANUAL"), WS_VISIBLE | WS_POPUP,
		710, 65, 80, 30, NULL, NULL, NULL, NULL);

	//initialize
	SendMessage(wow_on, BM_SETCHECK, BST_CHECKED, 1);
	SendMessage(woffw_on, BM_SETCHECK, BST_CHECKED, 1);
	SendMessage(ground_on, BM_SETCHECK, BST_CHECKED, 1);
	SendMessage(flight_on, BM_SETCHECK, BST_CHECKED, 1);
	EnableWindow(manual_mode, 0);
	EnableWindow(auto_mode, 1);
	buffer[0] = 1;
	buffer[1] = 1;
	buffer[2] = 1;
	buffer[3] = 1;

	SetParent(wow_on, main_control);
	SetParent(wow_off, main_control);
	SetParent(woffw_on, main_control);
	SetParent(woffw_off, main_control);
	SetParent(ground_on, main_control);
	SetParent(ground_off, main_control);
	SetParent(flight_on, main_control);
	SetParent(flight_off, main_control);
	SetParent(auto_mode, main_control);
	SetParent(manual_mode, main_control);
	SetParent(send_push, main_control);
	
	// Add color for main control window
	HDC hdc = BeginPaint(main_control, &ps);
	GetClientRect(main_control, &rc);
	SetDCBrushColor(hdc, bkcolor);
	FillRect(hdc, &rc, (HBRUSH)GetStockObject(DC_BRUSH));


	// Color the output text
	text = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(text, 7);
	printf("----------------------MANUAL MODE BEGIN--------------------");
	printf("\r\n");
	SetConsoleTextAttribute(text, FOREGROUND_RED);

	
	if (connected) {

		while (mode == 1 || GetMessage(&msg, 0, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			hlrm = (int)msg.wParam;
			handle_son = (int)msg.hwnd;
			bpressed = (hlrm == 1 || hlrm == 2 || hlrm == 16);
			
			// manual or auto 
			if (auto_mode_finish == 1 || (bpressed & handle_son == (int)manual_mode)) {
				EnableWindow(wow_on,1);
				EnableWindow(wow_off, 1);
				EnableWindow(woffw_on, 1);
				EnableWindow(woffw_off, 1);
				EnableWindow(ground_on, 1);
				EnableWindow(ground_off, 1);
				EnableWindow(flight_on, 1);
				EnableWindow(flight_off, 1);
				EnableWindow(send_push, 1);
				EnableWindow(manual_mode, 0);
				EnableWindow(auto_mode, 1);

				if (file_is_open == 1) {
					file_read.close();
					auto_mode_finish = 1;
					SetConsoleTextAttribute(text, 7);
					printf("----------------------AUTO MODE FINISH----------------------");
					printf("\r\n");
					printf("\r\n");
					printf("----------------------MANUAL MODE BEGIN---------------------");
					SetConsoleTextAttribute(text, FOREGROUND_RED);
					printf("\r\n");
					file_is_open = 0;
					EnableWindow(auto_mode, 0);
					EnableWindow(manual_mode, 1);
				}

				// adjust checks of the buttons after the auto mode finished 
				if (auto_mode_finish == 1) {
					if (buffer[0] == 1) {
						SendMessage(wow_on, BM_SETCHECK, BST_CHECKED, 1);
						SendMessage(wow_off, BM_SETCHECK, BST_UNCHECKED, 1);
					}
					else
					{
						SendMessage(wow_on, BM_SETCHECK, BST_UNCHECKED, 1);
						SendMessage(wow_off, BM_SETCHECK, BST_CHECKED, 1);
					}
					if (buffer[1] == 1) {
						SendMessage(woffw_on, BM_SETCHECK, BST_CHECKED, 1);
						SendMessage(woffw_off, BM_SETCHECK, BST_UNCHECKED, 1);
					}
					else
					{
						SendMessage(woffw_on, BM_SETCHECK, BST_UNCHECKED, 1);
						SendMessage(woffw_off, BM_SETCHECK, BST_CHECKED, 1);
					}
					if (buffer[2] == 1) {
						SendMessage(ground_on, BM_SETCHECK, BST_CHECKED, 1);
						SendMessage(ground_off, BM_SETCHECK, BST_UNCHECKED, 1);
					}
					else
					{
						SendMessage(ground_on, BM_SETCHECK, BST_UNCHECKED, 1);
						SendMessage(ground_off, BM_SETCHECK, BST_CHECKED, 1);
					}
					if (buffer[3] == 1) {
						SendMessage(flight_on, BM_SETCHECK, BST_CHECKED, 1);
						SendMessage(flight_off, BM_SETCHECK, BST_UNCHECKED, 1);
					}
					else
					{
						SendMessage(flight_on, BM_SETCHECK, BST_UNCHECKED, 1);
						SendMessage(flight_off, BM_SETCHECK, BST_CHECKED, 1);
					}
				}
				auto_mode_finish = 0;
				mode = 0;
				file_is_open = 0;
			}
			if (bpressed & handle_son == (int)auto_mode) {
				EnableWindow(wow_on,0);
				EnableWindow(wow_off, 0);
				EnableWindow(woffw_on, 0);
				EnableWindow(woffw_off, 0);
				EnableWindow(ground_on, 0);
				EnableWindow(ground_off, 0);
				EnableWindow(flight_on, 0);
				EnableWindow(flight_off, 0);
				EnableWindow(send_push, 0);
				EnableWindow(auto_mode, 0);
				EnableWindow(manual_mode, 1);
				mode = 1;
			}


			if (mode == 0) {

				// populate data in buffer
				if (handle_son == (int)wow_on & bpressed) {
					buffer[0] = 1;
					SendMessage(wow_on, BM_SETCHECK, BST_CHECKED, 1);
					SendMessage(wow_off, BM_SETCHECK, BST_UNCHECKED, 1);
					ischanging = 0;
				}
				if (handle_son == (int)wow_off & bpressed) {
					buffer[0] = 0;
					SendMessage(wow_on, BM_SETCHECK, BST_UNCHECKED, 1);
					SendMessage(wow_off, BM_SETCHECK, BST_CHECKED, 1);
					ischanging = 0;
				}

				if (handle_son == (int)woffw_on & bpressed) {
					buffer[1] = 1;
					SendMessage(woffw_on, BM_SETCHECK, BST_CHECKED, 1);
					SendMessage(woffw_off, BM_SETCHECK, BST_UNCHECKED, 1);
					ischanging = 0;
				}
				if (handle_son == (int)woffw_off & bpressed) {
					buffer[1] = 0;
					SendMessage(woffw_on, BM_SETCHECK, BST_UNCHECKED, 1);
					SendMessage(woffw_off, BM_SETCHECK, BST_CHECKED, 1);
					ischanging = 0;
				}

				if (handle_son == (int)ground_on & bpressed) {
					buffer[2] = 1;
					SendMessage(ground_on, BM_SETCHECK, BST_CHECKED, 1);
					SendMessage(ground_off, BM_SETCHECK, BST_UNCHECKED, 1);
					ischanging = 0;
				}
				if (handle_son == (int)ground_off & bpressed) {
					buffer[2] = 0;
					SendMessage(ground_on, BM_SETCHECK, BST_UNCHECKED, 1);
					SendMessage(ground_off, BM_SETCHECK, BST_CHECKED, 1);
					ischanging = 0;
				}

				if (handle_son == (int)flight_on & bpressed) {
					buffer[3] = 1;
					SendMessage(flight_on, BM_SETCHECK, BST_CHECKED, 1);
					SendMessage(flight_off, BM_SETCHECK, BST_UNCHECKED, 1);
					ischanging = 0;
				}
				if (handle_son == (int)flight_off & bpressed) {
					buffer[3] = 0;
					SendMessage(flight_on, BM_SETCHECK, BST_UNCHECKED, 1);
					SendMessage(flight_off, BM_SETCHECK, BST_CHECKED, 1);
					ischanging = 0;
				}

			}
			else
			{

				// start reading from file
				if (file_is_open == 0) {

				OPENFILENAME ofn;       // common dialog box structure
				TCHAR szFile[MAX_PATH];       // if using TCHAR macros

				// Initialize OPENFILENAME
				
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = NULL;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = NULL;
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		
				if (GetOpenFileName(&ofn)==TRUE)
			{
					for (int i = 0; i < sizeof(szFile); i++) {
						folder += szFile[i];
					}
					file_read.open(folder);

		 	}

					if (!file_read.is_open())
					{
						cout << "Unable to open the file." << endl;
					}
					else {
						SetConsoleTextAttribute(text, 7);
						printf("----------------------MANUAL MODE FINISH-------------------");
						printf("\r\n");
						printf("\r\n");
						printf("----------------------AUTO MODE BEGIN----------------------");
						SetConsoleTextAttribute(text, FOREGROUND_RED);
						printf("\r\n");
						file_is_open = 1;
						EnableWindow(auto_mode, 0);
                     	EnableWindow(manual_mode, 1);
					}
				}
				

				if (getline(file_read, line))
				{
					// initialize buffer
					buffer[0] = stoi(line.substr(0, 1));
					buffer[1] = stoi(line.substr(1, 1));
					buffer[2] = stoi(line.substr(2, 1));
					buffer[3] = stoi(line.substr(3, 1));
					time_elapsed = stoi(line.substr(5));
				}
				else {
					file_read.close();
					auto_mode_finish = 1;
					SetConsoleTextAttribute(text, 7);
					printf("----------------------AUTO MODE FINISH----------------------");
					printf("\r\n");
					printf("\r\n");
					printf("----------------------MANUAL MODE BEGIN---------------------");
					printf("\r\n");
					SetConsoleTextAttribute(text, FOREGROUND_RED);
					file_is_open = 0;
					mode = 0;
				}
			}


			if (mode == 0 & bpressed == 0 & check == 1 & handle_son == (int)send_push)
				check = 0;

			if ((mode == 1 & auto_mode_finish == 0) || (handle_son == (int)send_push & bpressed == 1 & check == 0)) {
				// print data
				printf("Sending DATA: ");
				for (i = 0; i < size; i++)
					printf("%d ", buffer[i]);
				if ((buffer[0] == 0 & buffer[1] == 0 & buffer[2] == 0 & buffer[3] == 0))
					printf("-> ID 1 (TRANSITORY)");
				if ((buffer[0] == 0 & buffer[1] == 0 & buffer[2] == 0 & buffer[3] == 1))
					printf("-> ID 2 (TRANSITORY)");
				if ((buffer[0] == 0 & buffer[1] == 1 & buffer[2] == 0 & buffer[3] == 0))
					printf("-> ID 3 (TRANSITORY)");
				if ((buffer[0] == 0 & buffer[1] == 1 & buffer[2] == 0 & buffer[3] == 1))
					printf("-> ID 4 (FLIGHT)");
				if ((buffer[0] == 1 & buffer[1] == 0 & buffer[2] == 0 & buffer[3] == 0))
					printf("-> ID 5 (GROUND)");
				if ((buffer[0] == 1 & buffer[1] == 0 & buffer[2] == 0 & buffer[3] == 1))
					printf("-> ID 6 (GROUND)");
				if ((buffer[0] == 1 & buffer[1] == 1 & buffer[2] == 0 & buffer[3] == 0))
					printf("-> ID 7 (TRANSITORY)");
				if ((buffer[0] == 1 & buffer[1] == 1 & buffer[2] == 0 & buffer[3] == 1))
					printf("-> ID 8 (TRANSITORY)");
				if ((buffer[0] == 0 & buffer[1] == 0 & buffer[2] == 1 & buffer[3] == 0))
					printf("-> ID 9 (GROUND)");
				if ((buffer[0] == 0 & buffer[1] == 0 & buffer[2] == 1 & buffer[3] == 1))
					printf("-> ID 10 (GROUND)");
				if ((buffer[0] == 0 & buffer[1] == 1 & buffer[2] == 1 & buffer[3] == 0))
					printf("-> ID 11 (GROUND)");
				if ((buffer[0] == 0 & buffer[1] == 1 & buffer[2] == 1 & buffer[3] == 1))
					printf("-> ID 12 (SPECIAL TRANSITORY)");
				if ((buffer[0] == 1 & buffer[1] == 0 & buffer[2] == 1 & buffer[3] == 0))
					printf("-> ID 13 (GROUND)");
				if ((buffer[0] == 1 & buffer[1] == 0 & buffer[2] == 1 & buffer[3] == 1))
					printf("-> ID 14 (GROUND)");
				if ((buffer[0] == 1 & buffer[1] == 1 & buffer[2] == 1 & buffer[3] == 0))
					printf("-> ID 15 (GROUND)");
				if ((buffer[0] == 1 & buffer[1] == 1 & buffer[2] == 1 & buffer[3] == 1))
					printf("-> ID 16 (GROUND)");
				check = 1;
				ischanging = 0;
				// send data
				if (sendto(s_s, (char*)buffer, size, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
				printf("sendto() failed with error code %d\r\n", WSAGetLastError());	
				if (sendto(s_s, (char*)buffer, size, 0, (struct sockaddr*)&servaddr_mon, sizeof(servaddr_mon)) == SOCKET_ERROR)
				printf("sendto() failed with error code %d\r\n", WSAGetLastError());
				auto start = std::chrono::system_clock::now();
				printf(" <--> Time:%d", start);
				printf("\r\n");
				if (mode == 1 & auto_mode_finish == 0)
				Sleep(time_elapsed);
			}
		}
	}
	return 0;
}