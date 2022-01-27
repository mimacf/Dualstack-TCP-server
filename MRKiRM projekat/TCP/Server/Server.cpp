#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include <in6addr.h>
#include <process.h>
#include <tchar.h>
#include <string.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_PORT 27015	// Port number of server that will be used for communication with clients
#define BUFFER_SIZE 512		// Size of buffer that will be used for sending and receiving messages to clients


//char velicina[BUFFER_SIZE + 1];
FILE* outFile;

char key[31] = "HELLOHELLOHELLOHELLOHELLOHELLO";
char decryptedMsg[31];

// Checks if ip address belongs to IPv4 address family
bool is_ipV4_address(sockaddr_in6 address);

typedef struct Adresa {
	SOCKET sock;
	sockaddr_in6 addr;
}Adresa;

unsigned int __stdcall ServClient(void *data);

int main()
{
	system("Color 0D");
	printf("########################################################################### \n");
	printf("# Server je aktivan!                                                         # \n");

	///////////////////////////////////////////////////////////////////// PRAVLJENJE IZLAZ.TXT FAJLA GDE SE UPISUJU PAKETI
	outFile = fopen("izlaz.txt", "wb");

	//////////////////////////////////////////////////////
    // Server address 
     sockaddr_in6  serverAddress; 

	// Buffer we will use to send and receive clients' messages
    char dataBuffer[BUFFER_SIZE];

	// WSADATA data structure that is to receive details of the Windows Sockets implementation
    WSADATA wsaData;

	// Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Initialize serverAddress structure used by bind function
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin6_family = AF_INET6; 			// set server address protocol family
    serverAddress.sin6_addr = in6addr_any;			// use all available addresses of server
    serverAddress.sin6_port = htons(SERVER_PORT);	// Set server port
	serverAddress.sin6_flowinfo = 0;				// flow info

    // Create a socket 
    SOCKET serverSocket = socket(AF_INET6,      // IPv6 address famly
								 SOCK_STREAM,    // datagram socket
								 IPPROTO_TCP);  // TCP
	SOCKET clientSocket;


	// Check if socket creation succeeded
    if (serverSocket == INVALID_SOCKET)
    {
        printf("Creating socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }


	// Disable receiving only IPv6 packets. We want to receive both IPv4 and IPv6 packets.
	/*char no = 0;     
	int iResult = setsockopt(serverSocket, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)); 
	
	if (iResult == SOCKET_ERROR) 
			printf("failed with error: %u\n", WSAGetLastError());
*/
	int iResult;

    // Bind server address structure (type, port number and local address) to socket
    iResult = bind(serverSocket,(SOCKADDR *)&serverAddress, sizeof(serverAddress));

	// Check if socket is succesfully binded to server datas
    if (iResult == SOCKET_ERROR)
    {
        printf("Socket bind failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

	iResult = listen(serverSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	//////accept
	sockaddr_in6 clientAddr;
	int sockAddrLen = sizeof(clientAddr);
	Adresa adresa;

	while (clientSocket = accept(serverSocket, (SOCKADDR *)&clientAddr, &sockAddrLen))
	{
		if (clientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}
		else
		{
			adresa.sock = clientSocket;
			adresa.addr = clientAddr;
			_beginthreadex(0, 0, ServClient, (void*)&adresa, 0, 0);
		}
	}

	return 0;

}

unsigned int __stdcall ServClient(void *data)
{
	Adresa* adresa = (Adresa *)data;
	Adresa clAdresa = *adresa;
	SOCKET ClientSocket = clAdresa.sock;
	sockaddr_in6 ClientAddres = clAdresa.addr;

	FILE* file;
	char fileName[20];

	char buff[31];

	int id = 1000;

	char ID[10];
	sprintf_s(ID, "%d", id);
	send(ClientSocket, ID, sizeof(ID), 0);

	////////////////// Convert IPv4 and IPv6 //////////////////
	char ipAddress[INET6_ADDRSTRLEN];

	// Copy client ip to local char[]
	inet_ntop(AF_INET6, &ClientAddres.sin6_addr, ipAddress, sizeof(ipAddress));

	// Convert port number from network byte order to host byte order
	unsigned short ClientPort = ntohs(ClientAddres.sin6_port);

	//true for IPv4 and false for IPv6
	bool isIPv4 = is_ipV4_address(ClientAddres);

	if (isIPv4)
	{
		char ipAddress4[15];
		inet_ntop(AF_INET, &ClientAddres, ipAddress4, sizeof(ipAddress4));
		printf("[IPv4] Client connected from --- PORT:[%d]\t IP:[%s]  ID:[%d]\n\n", ClientPort, ipAddress4, id);

	}
	else
	{
		printf("# [IPv6] Client connected from --- PORT:[%d]\t IP:[%s]  ID:[%d]      #\n", ClientPort, ipAddress, id);
	}
	printf("# Zapoceto primanje paketa!                                               # \n");
	printf("########################################################################### \n");
	printf("\n");
	int broj_paketa = 1;

	///////////////////////////////////////////////////////////////////// PRIJEM SIFROVANE PORUKE
	while (recv(ClientSocket, buff, sizeof(buff), 0))
	{
		int k;

		if (strcmp(buff, "exit") == 0)
		{
			printf("\nClient [%d] disconnect\n\n", id);
		}
		else
		{
			///////////////////////////////////////////////////////////////////// ISPIS SIFROVANE PORUKE
			buff[30] = '\0';
			printf("\033[32m Server message:");
			printf("\033[0m");
			printf(" prijem %d. paketa   Sadrzaj paketa (sifrovan paket): %s\n", broj_paketa, buff);
			printf("\n");

			sprintf_s(fileName, "%d", id);
			strcat_s(fileName, ".txt");
			fopen_s(&file, fileName, "w");
			fprintf(file, "%s", buff);
			fclose(file);

			// Server salje klijentu datoteku
			send(ClientSocket, buff, sizeof(buff), 0);

		}


		///////////////////////////////////////////////////////////////////// DEKRIPTOVANJE PORUKE
		for (k = 0; k < strlen(buff); ++k)
			decryptedMsg[k] = (((buff[k] - key[k]) + 26) % 26) + 'A';

		decryptedMsg[30] = '\0';


		///////////////////////////////////////////////////////////////////// PISANJE U FAJL
		fprintf(outFile, "%s", decryptedMsg);
		broj_paketa++;
	}

	return 0;
}

bool is_ipV4_address(sockaddr_in6 address)  
{
	char *check = (char*)&address.sin6_addr.u;

	for (int i = 0; i < 10; i++)
		if(check[i] != 0)
			return false;
		
	if(check[10] != -1 || check[11] != -1)
		return false;

	return true;
}