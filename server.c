#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>'

#define DEFAULT_PORT 2002

#define TEXT_SIZE 16384

void reverseString(char* str) {
	int length = strlen(str);
	int i;
	char temp;

	for (i = 0; i < length / 2; ++i) {
		temp = str[i];
		str[i] = str[length - i - 1];
		str[length - i - 1] = temp;
	}
}

int main() {

	setlocale(LC_ALL, "Russian");

	WSADATA wsaData;
	SOCKET listenSocket, clientSocket;
	struct sockaddr_in serverAddr, clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	char text[TEXT_SIZE];
	int bytesRead;

	// ������������� Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("������ ������������� Winsock\n");
		return 1;
	}

	// �������� ���������� ������
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		printf("������ �������� ���������� ������\n");
		WSACleanup();
		return 1;
	}

	// ��������� ������ �������
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(DEFAULT_PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	// �������� ���������� ������ � ������ �������
	if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		printf("������ �������� ���������� ������ � ������\n");
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// ������������� ������ �� ����������� �������
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("������ ������������� ������\n");
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	while (1) {
		// �������� ���������� �� �������
		clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
		if (clientSocket == INVALID_SOCKET) {
			printf("������ �������� ����������\n");
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		// ����� ������ �� ����������� ������
		bytesRead = recv(clientSocket, text, TEXT_SIZE - 1, 0);
		text[bytesRead] = '\0';

		// ��������� ������� ���� � ������
		reverseString(text);
		printf("���������� �����: %s\n", text);

		// �������� ����������� ������ ������� �������
		if (send(clientSocket, text, strlen(text), 0) == SOCKET_ERROR) {
			printf("������ �������� ������ �������\n");
			closesocket(clientSocket);
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		// �������� ����������
		closesocket(clientSocket);
	}

	closesocket(listenSocket);
	WSACleanup();

	return 0;
}