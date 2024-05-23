#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData;            // Структура для хранения информации о инициализации Winsock
    ADDRINFO hints;             // Структура для настройки getaddrinfo
    ADDRINFO* addrResult = nullptr; // Результат разрешения адреса
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для соединения
    char recvBuffer[512];       // Буфер для приема данных

    // Сообщения, которые клиент отправляет серверу
    const char* sendBuffer1 = "Hello from client 1";
    const char* sendBuffer2 = "Hello from client 2";

    // Инициализация Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    // Настройка структуры hints для getaddrinfo
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // Потоковый сокет (TCP)
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP

    // Разрешение адреса и порта сервера
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        WSACleanup();
        return 1;
    }

    // Создание сокета для подключения
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Отправка данных серверу
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    // Отправка второго сообщения серверу
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    // Остановка отправки данных
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Цикл приема данных от сервера
    do {
        ZeroMemory(recvBuffer, 512); // Очистка буфера приема
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Прием данных от сервера
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Connection closed" << endl; // Сервер закрыл соединение
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
        }
    } while (result > 0);

    // Очистка и завершение работы
    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}
