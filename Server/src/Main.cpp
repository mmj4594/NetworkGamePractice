#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA wsaData;
	SOCKET serverSocket, clientSocket;
	struct sockaddr_in serverAddr, clientAddr;
	int clientAddrSize = sizeof(clientAddr);

	// Windows Socket API Startup
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Create Server Socket(IPv4, TCP)
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(9000);
	bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

	// Wait for 1 Client
	listen(serverSocket, 1);
	std::cout << "Waiting for Client" << std::endl;

	// Accept Client
	clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
	std::cout << "Accepted Client!" << std::endl;

	// Receive Message from Client
	char buffer[1024] = { 0 };
	recv(clientSocket, buffer, sizeof(buffer), 0);
	std::cout << "Message from Client: " << buffer << std::endl;

	// Send Message to Client
	const char* message = "Hello, Client!";
	send(clientSocket, message, static_cast<int>(strlen(message)), 0);

	// Close Sockets
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}
