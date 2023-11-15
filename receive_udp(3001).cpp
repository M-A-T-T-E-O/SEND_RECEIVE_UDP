#include <stdio.h> 
#include "Ws2tcpip.h"
#include <stdlib.h> 
#include <time.h>
#include <string.h> 
#include <sys/types.h> 
#include <winsock2.h>
#include <chrono>
#include <ctime>
#include<iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996)

#define MAXLINE  4 


int main()
{
	// Server side implementation UDP
	int r_s, i, l, error;
	int connected = 1, size = 4;
	static WSADATA wsa;
	static unsigned char buffer[4];
	struct sockaddr_in clientaddr;
	u_long mode = 1;
	HANDLE text;

	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		printf("WSAStartup() failed with error code %d", WSAGetLastError());
		connected = 0;
	}

	r_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (r_s == SOCKET_ERROR) {
		printf("sending socket() failed with error code %d", WSAGetLastError());
		connected = 0;
	}

	ioctlsocket(r_s, FIONBIO, &mode);
	memset(&clientaddr, 0, sizeof(clientaddr));

	// Filling server information 
	clientaddr.sin_family = AF_INET; // IPv4 
	clientaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	clientaddr.sin_port = htons(3001);

	if (bind(r_s, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) == SOCKET_ERROR) {
		printf("bind() failed with error code : %d\n", WSAGetLastError());
		connected = 0;
	}


	// Color the output text
	text = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(text, FOREGROUND_RED);


	if (connected) {
		while (true) {
			l = recv(r_s, (char*)buffer, 4, 0);
			error = WSAGetLastError();

			if (error != 10035) {
				if (l == SOCKET_ERROR) {
					printf("recv() failed with error code : %d\n", error);
				}
				else {
					printf("Received: ");
					for (i = 0; i < 4; i++)
						printf("%d ", buffer[i]);
					auto stop = std::chrono::system_clock::now();
					printf(" <--> Time:%d", stop);
					printf("\r\n");
				}
			}


		}
	}
	return 0;
}