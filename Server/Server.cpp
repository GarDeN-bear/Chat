#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

const int MAX_CLIENTS = 64;

SOCKET Connect; // сокет для приёма подключаемых пользователей
SOCKET connections[MAX_CLIENTS]; // коллекция id пользователей (все пользователи на сервере)
SOCKET Listen; // сокет для подключения
SOCKADDR_IN name_conn[MAX_CLIENTS]; // коллекция имён пользователей (все пользователи на сервере)

int ClientCount = 0; // количество клиентов на сервере

// Функция отправки сообщения клиенту по ID
void SendMessageToClient(int);

int main() {
    setlocale(LC_ALL, "Russian"); // установка русского языка
    char port[10]; // переменная, в которую передаётся информация о порте сервера
    WSAData data;  // переменная, в которую загружаются данные о версии сокетов
    WORD version = MAKEWORD(2, 2); // версия сокетов
    int res = WSAStartup(version, &data); // инициализация сокетов
    if (res != 0) { // это плохо, т.к. сокеты не инициализированы
        return 0;
    }
    struct addrinfo hints; // переменные для работы с сокетами
    struct addrinfo* result; // переменные для работы с сокетами
    ZeroMemory(&hints, sizeof(hints)); // очищение структуры
    hints.ai_family = AF_INET; // инициализация полей hints
    hints.ai_flags = AI_PASSIVE; // инициализация полей hints
    hints.ai_socktype = SOCK_STREAM; // инициализация полей hints
    hints.ai_protocol = IPPROTO_TCP; // инициализация полей hints
    printf("Input port: "); 
    std::cin >> port; // инициализация порта хоста
    getaddrinfo(NULL, port, &hints, &result); // информация о хосте
    Listen = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // настройка Listen
    bind(Listen, result->ai_addr, result->ai_addrlen); // объявление сервера в интернете
    listen(Listen, SOMAXCONN); // передача сокета, который будет ждать подключение (максимальное количество)
    freeaddrinfo(result); // удаление всей информации, т.к. сокеты настроены
    printf("Server started\n");
    char m_connect[] = "Connected in test server"; // передача клиенту информации о том, что он подключился
    //запуск бесконечного цикла
    for (;; Sleep(75)) {
        int size = sizeof(name_conn[ClientCount]);
        if (Connect = accept(Listen, (SOCKADDR*)&name_conn[ClientCount], &size)) { // проверка на подключение клиента к серверу
            printf("Client connected(%s)\n", inet_ntoa(name_conn[ClientCount].sin_addr)); // клиент подключился
            connections[ClientCount] = Connect; // сохранение сокета клиента
            send(connections[ClientCount], m_connect, strlen(m_connect), 0); // отправка сообщения клиенту о его подключении
            ClientCount++; // увеличение числа пользователей на сервере
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendMessageToClient, (LPVOID)(ClientCount - 1), NULL, NULL); // передача сообщений клиентов через сервер
        }
    }
}

void SendMessageToClient(int ID) {
    char* buffer = new char[1024]; // буффер временного сообщения
    for (;; Sleep(75)) {
        for (int i = 0; i < 1024; i++) buffer[i] = 0; // очищение буффера
        if (recv(connections[ID], buffer, 1024, 0) > 0) { // получение сообщения от пользователя и передача его в буффер
            printf(buffer); // вывод сообщения на сервере
            printf("\n");
            for (int i = 0; i <= ClientCount; i++) {
                send(connections[i], buffer, strlen(buffer), 0); // передача сообщения всем клиентам на сервере
            }
        }
        else break;
    }
    printf("Client disconnected(%s)\n", inet_ntoa(name_conn[ID].sin_addr)); // если клиент отключился, то из сервера удалюятся его данные
    for (int i = ID; i < ClientCount - 1; i++) {
        connections[i] = connections[i + 1];
        name_conn[i] = name_conn[i + 1];
    }
    ClientCount--;
    delete buffer; // удаление буффера из динамической памяти
}