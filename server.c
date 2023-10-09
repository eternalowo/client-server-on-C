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

	// Инициализация Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("Ошибка инициализации Winsock\n");
		return 1;
	}

	// Создание слушающего сокета
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		printf("Ошибка создания слушающего сокета\n");
		WSACleanup();
		return 1;
	}

	// Настройка адреса сервера
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(DEFAULT_PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	// Привязка слушающего сокета к адресу сервера
	if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		printf("Ошибка привязки слушающего сокета к адресу\n");
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// Прослушивание сокета на подключение клиента
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Ошибка прослушивания сокета\n");
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	while (1) {
		// Принятие соединения от клиента
		clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
		if (clientSocket == INVALID_SOCKET) {
			printf("Ошибка принятия соединения\n");
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		// Прием данных из клиентского сокета
		bytesRead = recv(clientSocket, text, TEXT_SIZE - 1, 0);
		text[bytesRead] = '\0';

		// Изменение порядка букв в тексте
		reverseString(text);
		printf("Полученный текст: %s\n", text);

		// Отправка измененного текста обратно клиенту
		if (send(clientSocket, text, strlen(text), 0) == SOCKET_ERROR) {
			printf("Ошибка отправки данных клиенту\n");
			closesocket(clientSocket);
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		// Закрытие соединения
		closesocket(clientSocket);
	}

	closesocket(listenSocket);
	WSACleanup();

	return 0;
}