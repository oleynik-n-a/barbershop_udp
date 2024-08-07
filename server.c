#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>

#define BACKLOG 10
#define LOGGERS 10
#define MSG_SIZE 128

int client_number;
int log_sockfd;
char *ip;
char *clients_port;
char *loggers_port;
int is_message;
char *message;

void *start_logger_thread(void *args) {
    log_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (log_sockfd == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in logaddr;
    socklen_t logaddr_len = sizeof(logaddr);
    logaddr.sin_family = AF_INET;
    logaddr.sin_addr.s_addr = inet_addr(ip);
    logaddr.sin_port = htons(atoi(loggers_port));

    if (bind(log_sockfd, (struct sockaddr *) &logaddr, sizeof(logaddr)) == -1) {
        perror("bind() error");
        exit(EXIT_FAILURE);
    }

    char buffer1[256];
    // Ожидание ответа от логгера
    int n = recvfrom(log_sockfd, buffer1, sizeof(buffer1), 0, (struct sockaddr *) &logaddr, &logaddr_len);
    if (n == -1) {
        perror("recvfrom() error");
        exit(EXIT_FAILURE);
    }

    // Принятие входящего соединения
    while (1) {
        if (is_message) {
            if (is_message == 2) {
                char buffer[256];
                snprintf(buffer, sizeof(buffer), "%d%s", client_number, message);
                int n = sendto(log_sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &logaddr, logaddr_len);
                if (n == -1) {
                    perror("sendto1() error");
                    exit(EXIT_FAILURE);
                }
            } else if (is_message == 3) {
                char buffer[256];
                snprintf(buffer, sizeof(buffer), "%s%d\n", message, client_number);
                int n = sendto(log_sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &logaddr, logaddr_len);
                if (n == -1) {
                    perror("sendto2() error");
                    exit(EXIT_FAILURE);
                }
            } else {
                int n = sendto(log_sockfd, message, strlen(message), 0, (struct sockaddr *) &logaddr, logaddr_len);
                if (n == -1) {
                    perror("sendto3() error");
                    exit(EXIT_FAILURE);
                }
            }
            message = "*";
            is_message = 0;
        }
    }

    return (NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <IP address> <clients_port> <loggers_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    client_number = 1;
    is_message = 0;
    message = "";
    ip = argv[1];
    clients_port = argv[2];
    loggers_port = argv[3];

    // Создание сокета
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    // Связывание сокетов с адресами
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(atoi(clients_port));

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        perror("bind() error");
        exit(EXIT_FAILURE);
    }

    pthread_t logger_thread;

    if (pthread_create(&logger_thread, NULL, start_logger_thread, NULL) == -1) {
        perror("pthread_create() error");
        exit(EXIT_FAILURE);
    }

    // Начало рабочего дня. Парикмахер спит в кресле
    printf("\n");
    printf("================================================\n");
    printf(" Начало рабочего дня - парикмахер спит в кресле \n");
    printf("================================================\n\n");

    printf("Парикмахер спит\n\n");
    message = "Парикмахер спит\n\n";
    is_message = 1;

    // Индикатор текущего состояния парикмахера
    int sleeping = 1;

    // Цикл обработки клиентов
    while (1) {
        struct pollfd pfd;
        pfd.fd = sockfd;
        pfd.events = POLLIN;

        // Парикмахер решил поспать, пока в очереди никого нет
        if (poll(&pfd, 1, 1) == 0 && sleeping == 0) {
            sleeping = 1;
            printf("Парикмахер уснул в кресле\n\n");
            message = "Парикмахер уснул в кресле\n\n";
            is_message = 1;
        }

        // Прием сообщения от клиента
        char buffer[1024];
        struct sockaddr_in cliaddr;
        socklen_t clilen = sizeof(cliaddr);

        // Ожидание ответа от клиента
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cliaddr, &clilen);
        if (n == -1) {
            perror("recvfrom() error");
            exit(EXIT_FAILURE);
        }

        poll(&pfd, 1, 1);
        message = " клиент встал в очередь\n";
        is_message = 2;

        // Отправка сообщения клиенту о том, что парикмахер спит
        strcpy(buffer, "Парикмахер спит");
        n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &cliaddr, clilen);
        if (n == -1) {
            perror("sendto4() error");
            exit(EXIT_FAILURE);
        }

        // Ожидание ответа от клиента
        n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cliaddr, &clilen);
        if (n == -1) {
            perror("recvfrom() error");
            exit(EXIT_FAILURE);
        }

        poll(&pfd, 1, 1);
        if (sleeping == 1) {
            // Парикмахер начинает стричь
            printf("Парикмахер проснулся и встал с кресла\n");
            message = " клиент разбудил парикмахера\n";
            is_message = 2;
            poll(&pfd, 1, 1);
            message = "Парикмахер проснулся и встал с кресла\n";
            is_message = 1;
            sleeping = 0;
        }

        // Ожидание информации о времени стрижки от клиента
        n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cliaddr, &clilen);
        if (n == -1) {
            perror("recvfrom() error");
            exit(EXIT_FAILURE);
        }

        // Преобразование времени стрижки из строки в целое число
        int haircut_time = atoi(buffer);

        // Задержка выполнения на время стрижки
        poll(&pfd, 1, 1);
        printf("Парикмахер стрижет клиента %d\n", client_number);
        message = "Парикмахер стрижет клиента ";
        is_message = 3;
        poll(&pfd, 1, 1);
        message = " клиент стрижется\n";
        is_message = 2;
        sleep(haircut_time);

        // Парикмахер закончил стрижку
        poll(&pfd, 1, 1);
        printf("Парикмахер закончил стрижку клиента %d\n", client_number);
        message = "Парикмахер закончил стрижку клиента ";
        is_message = 3;

        // Отправка сообщения клиенту о завершении стрижки
        poll(&pfd, 1, 1);
        strcpy(buffer, "Клиент постригся и ушел");
        message = " клиент постригся и ушел\n";
        is_message = 2;
        n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &cliaddr, clilen);
        if (n == -1) {
            perror("sendto5() error");
            exit(EXIT_FAILURE);
        }
        ++client_number;
    }

    // Закрытие сокета сервера
    close(sockfd);

    return 0;
}
