#include <WinSock2.h>
#include <iostream>
#include <ctime>
#include <WS2tcpip.h>

#include <cstring>

SOCKET Client; // сокет для полкдючения клиента

char name[24]; // имя клиента

// Функция отправки сообщения клиентам на сервере
void SendMessageToClient();


int main() {
    // как и в коде для сервера происходит инициализация сокета клиента с использованием данных из файла info.txt
    system("chcp 1251"); system("cls");
    char message[1024]; // переменная для передачи сообщения клиента на сервер
    char in[1024]; // переменная для получения текста, введённого клиентом
    char adrserv[INET_ADDRSTRLEN]; // переменная для получения IP адреса клиента
    FILE* f = fopen("info.txt", "r"); // чтение данных из тестовика
    if (!f)
        return 321;
    int port; // переменная, в которую передаётся информация о порте сервера
    WSAData data; // переменная, в которую загружаются данные о версии сокетов
    WORD version = MAKEWORD(2, 2); // версия сокетов
    int res = WSAStartup(version, &data);  // инициализация сокетов
    if (res != 0) { // это плохо, т.к. сокеты не инициализированы
        return 0;
    }
    Client = socket(AF_INET, SOCK_STREAM, 0); // настройка сокета клиента
    if (Client < 0) { // если что-то пошло не так, то получаем пояснение к ошибке
        printf("Socket() error %d\n", WSAGetLastError());
        return -1;
    }
    printf("Inputed port: ");
    fscanf(f, "%d\n", &port); // инициализация информации о порте сервера из текстовика
    std::cout << port << std::endl;
    printf("Inputed ip: ");
    fscanf(f, "%s\n", &adrserv); // инициализация IP адреса клиента из текстовика
    std::cout << adrserv << std::endl;
    struct sockaddr_in connect_c; 
    connect_c.sin_family = AF_INET; // установка адреса family
    connect_c.sin_port = htons(port); // установка порта
    HOSTENT* hst;
    if (inet_addr(adrserv) != INADDR_NONE) // подключение клиента к серверу с обработкой возможных ошибок
        connect_c.sin_addr.s_addr = inet_addr(adrserv);
    else {
        if (hst = gethostbyname(adrserv)) // если порт хоста из тестовика совпадает с портом сервера, то происходит подключение клиента к серверу
            ((unsigned long*)&connect_c.sin_addr)[0] =
            ((unsigned long**)hst->h_addr_list)[0][0];
        else {
            printf("Invalid address: %s\n", adrserv);
            closesocket(Client);
            WSACleanup();
            return -1;
        }
    }
    if (connect(Client, (sockaddr*)&connect_c, sizeof(connect_c))) { // если что-то пошло не так, то получаем пояснение к ошибке
        printf("Connect error %d\n", WSAGetLastError());
        return -1;
    }
    printf("Connect is OK!\n");
    printf("Inputed name: ");
    fscanf(f, "%s\n", &name);  // инициализация имени клиента из текстовика
    std::cout << name << std::endl;
    fclose(f); // закрытие тестового файла
    strcat(name, ": "); // присоединение к имени двоеточия для дальнейшей передачи сообщения
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendMessageToClient, CreateMutex(NULL, FALSE, NULL), NULL, NULL); // передача сообщений клиенту через сервер
    do {
        for (int i = 0; i < 1024; i++) message[i] = 0; // очищение массива сообщения
        strcpy(message, name); // передача имени в массив сообщения
        fflush(stdin); // очистка потока ввода
        gets_s(in); // ввод текста пользователем
        if (strcmp(in, "exit") == 0) break;  // отключение клиента от сервера, если клиент ввёл слово exit
        strcat(message, in); // передача введённого сообщения в массив сообщения
        send(Client, message, strlen(message), 0); // отправление сообщения от клиента на сервер
    } while (1);
    closesocket(Client); // когда клиент отключается от сервера необходимо закрыть его сокет
    WSACleanup(); // сборка мусора
    return 0;
}

void SendMessageToClient() {
    char out[1024];
    for (;; Sleep(75)) {
        for (int i = 0; i < 1024; i++) out[i] = 0;
        if (recv(Client, out, 1024, 0) > 0) { // получение сообщения от сервера и передача его в буффер
            printf(out);
            printf("\n");
        }
    }
}