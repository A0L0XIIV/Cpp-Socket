/*Baran Kaya*/

/**************************
* Basic socket program	  *
* Server - Client		  *
* Local IP				  *
**************************/

#include "stdafx.h"

#include <iostream>
#include <cstdlib>
#include <winsock2.h>
#include <ws2tcpip.h>

//WinSock Documentation
//https://docs.microsoft.com/en-us/windows/desktop/winsock

//The WSAStartup function is called to initiate use of WS2_32.dll.
#pragma comment(lib, "Ws2_32.lib")

//Port to run
#define DEFAULT_PORT "27015"

//Data buffer length
#define DEFAULT_BUFLEN 512

using namespace std;

struct addrinfo *result = NULL, *ptr = NULL, hints;

int main()
{
	WSADATA wsaData;

	// Result of the steps & for checking the errors.
	int iResult;
	
	// 0. Initialize Winsock
	try {
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	}
	catch (exception& e) {
		cout << "WSAStartup exception: " << e.what() << endl;
	}
	if (iResult != 0) {
		cout << "WSAStartup\t\t\tFAILED: " << iResult << endl;
		return 1;
	}
	else {
		cout << "WSAStartup\t\t\tSUCCESS" << endl;
	}


	// 1. Create a socket for server
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		cout << "Getaddrinfo\t\t\tFAILED: " << iResult << endl;
		WSACleanup();
		return 1;
	}

	// SOCKET object for the server to listen for client connections
	SOCKET ListenSocket = INVALID_SOCKET;

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		cout << "Listen Socket\t\tFAILED: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	else {
		cout << "ListenSocket\t\t\tSUCCESS" << endl;
	}


	// 2. Bind a socket - Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		cout << "Bind\t\t\t\tFAILED: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Bind\t\t\t\tSUCCESS" << endl;
	}

	// addr. info returned by the getaddrinfo function is no longer needed
	freeaddrinfo(result);


	// 3. Listen on socket
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen\t\t\t\tSUCCESS: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Listen\t\t\t\tSUCCESS" << endl;
	}


	// 4. Accept connection
	// Temp SOCKET for accepting connections from clients
	SOCKET ClientSocket;

	ClientSocket = INVALID_SOCKET;

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		cout << "Accept Client\t\t\t\tFAILED" << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Accept Client\t\t\t\tSUCCESS" << endl;
	}


	// 5. Recieve&Send data
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);


	// 6. Disconnect the server-shutdown the connection
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "Shutdown\t\t\t\tFAILED" << WSAGetLastError() << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Shutdown\t\t\t\tSUCCESS" << endl;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();


    return 0;
}

