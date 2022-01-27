#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include <tchar.h>
#include <string>
#include <string.h>
#include <iostream>



#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_IP_ADDRESS "::1"				// IPv6 address of server in localhost
#define SERVER_PORT 27015					// Port number of server that will be used for communication with clients
#define BUFFER_SIZE 512						// Size of buffer that will be used for sending and receiving messages to client
#define FILE_NAME "tekst.txt"
#define START_BUFF 20 
#define PACKAGE_SIZE 30
#define MAX_PACKAGES 20

int number_of_packages;  //parts of file
int sendResult;
int conffd;
FILE* fileForSend;
char velicina[BUFFER_SIZE + 1];
char key[31] = "HELLOHELLOHELLOHELLOHELLOHELLO";
char encryptedMsg[31];
char decryptedMsg[31];
char proba[30];

int main()
{
	system("Color 0D");

	printf("############################ \n");
	printf("# Klijent aktivan!         # \n");



	int irez;
	int duzina;
	int k;
	int c;
	int brojac = 0;
	int p;

	char startBuffer[START_BUFF];

	///////////////////////////////////////////////////////////////////// OTVARANJE FAJLA SA TEKSTOM
	fileForSend = fopen(FILE_NAME, "rb");
	if (fileForSend == NULL) {
		printf("Can't open the file %s", FILE_NAME);
	}
	else {
		printf("# Fajl otvoren!            # \n");
		fseek(fileForSend, 0, SEEK_END);
		duzina = ftell(fileForSend);
	}

	///////////////////////////////////////////////////////////////////// PAKOVANJE SADRZAJA FAJLA U NIKOLA
	number_of_packages = duzina / PACKAGE_SIZE + 1;
	printf("# Broj paketa za slanje %d  # \n", number_of_packages);
	fseek(fileForSend, 0 * BUFFER_SIZE/**i*/, SEEK_SET);
	fread(velicina, sizeof(char), BUFFER_SIZE - 1, fileForSend);

	
	///////////////////////////////////////////////////////////////////// PAKOVANJE SVIH PAKETA U NIZ PAKETA
	char packages[MAX_PACKAGES][PACKAGE_SIZE];

	for (k = 0; k < number_of_packages; k++)
	{
		for (c = 0; c < PACKAGE_SIZE; c++)
		{
			packages[k][c] = velicina[brojac];
			brojac++;
		}
	}

	
    // Server address structure
    sockaddr_in6 serverAddress;

    // Size of server address structure
	int sockAddrLen = sizeof(serverAddress);

	// Buffer that will be used for sending and receiving messages to client
    char dataBuffer[BUFFER_SIZE];

	// WSADATA data structure that is used to receive details of the Windows Sockets implementation
    WSADATA wsaData;
    
	// Initialize windows sockets for this process
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    
	// Check if library is succesfully initialized
	if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

   // Initialize memory for address structure
    memset((char*)&serverAddress, 0, sizeof(serverAddress));		
    
	
	 // Initialize address structure of server
	serverAddress.sin6_family = AF_INET6;								// IPv6 address famly
    inet_pton(AF_INET6, SERVER_IP_ADDRESS, &serverAddress.sin6_addr);	// Set server IP address using string
    serverAddress.sin6_port = htons(SERVER_PORT);						// Set server port
	serverAddress.sin6_flowinfo = 0;									// flow info
	 

	// Create a socket
    SOCKET clientSocket = socket(AF_INET6,      // IPv6 address famly
								 SOCK_STREAM,    // Datagram socket
								 IPPROTO_TCP);  // TCP protocol

	// Check if socket creation succeeded
    if (clientSocket == INVALID_SOCKET)
    {
        printf("Creating socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
	
	/////connecting////
	iResult = connect(clientSocket, (SOCKADDR *)&serverAddress, sizeof(serverAddress));
	if (iResult)
	{
		printf("Connect failed (ipv6) %u\n", WSAGetLastError());
		return 2;
	}
	
	printf("# Klijent IPv6 konektovan  # \n");
	char buff[512];
	char import[512];
	FILE* file;
	char fileName[20] = "download";
	char ID[100];

	// Client get ID from Server
	recv(clientSocket, ID, sizeof(ID), 0);
	printf("# Vas ID je: [%s]        # \n", ID);
	// Make "file.txt"
	strcat_s(fileName, ID);
	strcat_s(fileName, ".txt");

	int broj_paketa = 1;
	int kraj = 0;
	char paket[31];

	printf("# Slanje paketa zapoceto!  # \n");
	printf("############################ \n");
	printf("\n");

	while(kraj != number_of_packages)
	{
		
		printf("\033[36m Client message:");
		printf("\033[0m");
		printf(" %d. paket poslat \n", broj_paketa);
		printf("\n");
		//printf("\033[0m");
		
		///////////////////////////////////////////////////////////////////// PRAVLJENJE ZASEBNOG PAKETA
		for (p = 0; p < PACKAGE_SIZE; p++)
		{
			paket[p] = packages[kraj][p];
		}
		paket[30] = '\0';

		///////////////////////////////////////////////////////////////////// SIFROVANJE PAKETA
		for (k = 0; k < strlen(paket); ++k)
			encryptedMsg[k] = ((paket[k] + key[k]) % 26) + 'A';

		encryptedMsg[30] = '\0';

		

		///////////////////////////////////////////////////////////////////// SLANJE SIFROVANOG PAKETA
		iResult = sendto(clientSocket,						// Own socket
						 encryptedMsg,					// Text of message
						 sizeof(encryptedMsg),						// Message size
						 0,									// No flags
						 (SOCKADDR *)&serverAddress,		// Address structure of server (type, IP address and port)
						 sizeof(serverAddress));			// Size of sockadr_in structure

		// Check if message is succesfully sent. If not, close client application
		if (iResult == SOCKET_ERROR)
		{
			printf("sendto failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

		broj_paketa++;
		kraj++;

		Sleep(1500);
	}

	// Korisnik preuzima datoteku sa servera
	recv(clientSocket, import, sizeof(import), 0);
	fopen_s(&file, fileName, "w");
	fprintf(file, "%s", import);
	fclose(file);

	// Only for demonstration purpose
	printf("Press any key to exit: ");
	_getch();

	// Close client application
    iResult = closesocket(clientSocket);
    if (iResult == SOCKET_ERROR)
    {
        printf("closesocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
        return 1;
    }

	// Close Winsock library
    WSACleanup();

    return 0;
}
