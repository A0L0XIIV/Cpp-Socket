/*Baran Kaya*/

/**************************
* Basic Windows socket    *
* Server - Client		  *
* Local IP				  *
**************************/

//#include "stdafx.h"

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread> // For threads

#include "Map.h"
#include "WaitingRoom.h"
#include "Player.h"

// WinSock Documentation
// https://docs.microsoft.com/en-us/windows/desktop/winsock

// The WSAStartup function is called to initiate use of WS2_32.dll.
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

struct addrinfo *result = NULL;
struct addrinfo hints;

void ClientSession(const SOCKET& ListenSocket, vector<SOCKET>& ClientSockets, SOCKET newCl, char * recvbuf);

int main()
{
	WSADATA wsaData;

	// Result of the steps & for checking the errors.
	int iResult = 0;
	// Port to run
	const char* DEFAULT_PORT = "27015";
	// Data buffer length
	const size_t DEFAULT_BUFLEN = 512;
	// Max client number
	const size_t MAX_CLIENT = 10;
	size_t clientCounter = 0;

	// SOCKET object for the server to listen&send for client connections
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET NewClientSocket = INVALID_SOCKET;
	vector<SOCKET> ClientSockets;

	// Recieve variables
	char recvbuf[DEFAULT_BUFLEN] = {};
	int iSendResult = 0;
	int recvbuflen = DEFAULT_BUFLEN;
	
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

	// Create a SOCKET for connecting to server
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
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		cout << "Listen\t\t\t\tSUCCESS: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Listen\t\t\t\tSUCCESS" << endl;
	}

	/*
	// 4. Accept connection - Accept a client socket
	NewClientSocket = accept(ListenSocket, NULL, NULL);
	if (NewClientSocket == INVALID_SOCKET) {
		cout << "Accept Client\t\t\tFAILED" << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Accept Client\t\t\tSUCCESS" << endl;
		Player *player1 = new Player("Client 1", 'O');
	}


	// 5. Recieve&Send data - Receive until the peer shuts down the connection
	do {
		iResult = recv(NewClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			cout << "Bytes received: " << iResult << endl << "Recieved Message: ";
			for(int i = 0; i < iResult; ++i)
				cout << recvbuf[i];
			cout << endl << flush;

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				cout << "Send failed: " << WSAGetLastError() << endl;
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0) {
			cout << "Connection closing..." << endl;
			Sleep(1000);
			cout << "3" << endl << flush;
			Sleep(1000);
			cout << "2" << endl << flush;
			Sleep(1000);
			cout << "1" << endl << flush;
			Sleep(1000);
		}
		else {
			cout << "Recieve failed: " << WSAGetLastError() << endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);*/
	SOCKET client_socket;
	//while ((client_socket = accept(ListenSocket, NULL, NULL)) && clientCounter <= MAX_CLIENT) {
	while (1) {
		// Sleep a second
		Sleep(1000);
		// New client arrived
		if ((client_socket = accept(ListenSocket, NULL, NULL)) && clientCounter <= MAX_CLIENT) {
			// Create and execute the thread
			thread* clientHandleThread = new thread(ClientSession, std::ref(ListenSocket), std::ref(ClientSockets), client_socket, recvbuf);
			// Without detach, recv function doesn't work
			clientHandleThread->detach();
			// Increase counter
			++clientCounter;
		}
		// No new clients or max number of clients: do nothing
		else
			continue;
	}


	// 6. Disconnect the server-shutdown the connection
	iResult = shutdown(ListenSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "Shutdown\t\t\tFAILED" << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Shutdown\t\t\tSUCCESS" << endl;
	}

	// 7. Cleanup
	closesocket(ListenSocket);
	WSACleanup();

    return 0;
}


// Individual Client Handle Thread Function
void ClientSession(const SOCKET& ListenSocket, vector<SOCKET>& ClientSockets, SOCKET newCl, char* recvbuf) {
	const size_t DEFAULT_BUFLEN = 512;
	int iResult = 0;
	// Recieve variables
	//char recvbuf[DEFAULT_BUFLEN] = {};
	int iSendResult = 0;
	int recvbuflen = DEFAULT_BUFLEN;
	

	// 4. Accept connection - Accept a client socket
	SOCKET NewClientSocket = newCl;//accept(ListenSocket, NULL, NULL);

	if (NewClientSocket == INVALID_SOCKET) {
		cout << "Accept Client " << NewClientSocket << "\t\tFAILED" << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	else {
		cout << "Accept Client " << NewClientSocket << "\t\tSUCCESS" << endl;
		// Add Socket in the client sockets vector
		ClientSockets.push_back(NewClientSocket);
		// Create a new player object
		Player* player1 = new Player("Client 1", 'O');
		// TODO: Hold players in a vector like sockets
	}

	
	// 5. Recieve&Send data - Receive until the peer shuts down the connection
	do {
		iResult = recv(NewClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			cout << iResult << " bytes received from " << NewClientSocket << ", message: ";
			for (int i = 0; i < iResult; ++i)
				cout << recvbuf[i];
			cout << endl << flush;

			// Echo the buffer to all clients
			for (SOCKET clientSocket : ClientSockets) {
				iSendResult = send(clientSocket, recvbuf, iResult, 0);
				if (iSendResult == SOCKET_ERROR) {
					cout << "Send failed to " << clientSocket << ": " << WSAGetLastError() << endl;
					closesocket(clientSocket);
					WSACleanup();
					// Find the socket inside the vector
					auto toErease = std::find(ClientSockets.begin(), ClientSockets.end(), clientSocket);
					// And then erase if found
					if (toErease != ClientSockets.end()) {
						// Delete from the clients vector
						ClientSockets.erase(toErease);
					}
					return;
				}
				cout << "Bytes sent to " << clientSocket << ": " << iSendResult << endl << flush;
			}
		}
		else if (iResult == 0) {
			cout << "Player " << NewClientSocket << " has left the server." << endl;
		}
		else {
			cout << "Recieve failed: " << WSAGetLastError() << endl;
			closesocket(NewClientSocket);
			WSACleanup();
			// Find the socket inside the vector
			auto toErease = std::find(ClientSockets.begin(), ClientSockets.end(), NewClientSocket);
			// And then erase if found
			if (toErease != ClientSockets.end()) {
				// Delete from the clients vector
				ClientSockets.erase(toErease);
			}
			return;
		}
	} while (iResult > 0);
}