#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#define DEFAULT_PORT 2002

#define BUFFER_SIZE 1024
#define TEXT_SIZE 16384

char buffer[BUFFER_SIZE];
char text[TEXT_SIZE];

void get_text() {
	int empty_lines_count = 0; 
	do {
		fgets(buffer, BUFFER_SIZE, stdin);
		if (strcmp(buffer, "\n") == 0) { 
			empty_lines_count++;
		}
		else {
			empty_lines_count = 0;
		}
		strcat_s(text, TEXT_SIZE, buffer);
	} while (empty_lines_count < 1);
}

int main() {
	
	setlocale(LC_ALL, "Russian");

	WSADATA wsaData;
	SOCKET clientSocket;
	struct sockaddr_in serverAddr;

	int bytesRead;
	char ip_input[16] = "\0";
	char port_input[6];
	char* endptr;
	size_t port = DEFAULT_PORT;

	printf("Введите IP-адрес сервера: ");
	fgets(ip_input, 16, stdin);

	printf("Введите порт сервера: ");
	fgets(port_input, 6, stdin);
	port_input[strcspn(port_input, "\n")] = '\0';

	port = strtol(port_input, &endptr, 10);

	if (!(port >= 1024 && port <= 65535)) {
		printf("Некорректный порт");
		return 1;
	}

	// Инициализация Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("Ошибка инициализации Winsock\n");
		return 1;
	}
	
	// Создание клиентского сокета
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		printf("Ошибка создания клиентского сокета\n");
		WSACleanup();
		return 1;
	}

	// Настройка адреса сервера
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ip_input);


	// Подключение к серверу
	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		printf("Ошибка подключения к серверу\n");
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	// Ввод текста с клавиатуры
	printf("Введите текст: ");
	get_text();

	// Отправка текста серверу
	if (send(clientSocket, text, strlen(text), 0) == SOCKET_ERROR) {
		printf("Ошибка отправки данных серверу\n");
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	// Прием измененного текста от сервера
	bytesRead = recv(clientSocket, text, TEXT_SIZE - 1, 0);
	text[bytesRead] = '\0';
	printf("Измененный текст от сервера: %s\n", text);

	// Закрытие соединения
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}