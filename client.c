#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 

#define BACKLOG 10

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <IP address> <port> <time>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Создание сокета
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    // Подключение к серверу
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    printf("\n");
    printf("================================\n");
    printf(" Клиент пришел в парикмахерскую \n");
    printf("================================\n\n");

    char buffer[1024];
    socklen_t servaddr_len = sizeof(servaddr);

    // Отправка сообщения серверу
    strcpy(buffer, "");
    int n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (n == -1) {
        perror("sendto() error");
        exit(EXIT_FAILURE);
    }

    // Отправка сообщения серверу
    strcpy(buffer, "Клиент встал в очередь");
    n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (n == -1) {
        perror("sendto() error");
        exit(EXIT_FAILURE);
    }

    // Клиент встал в очередь
    printf("Клиент встал в очередь\n");

    // Ожидание сообщения от сервера о пробуждении парикмахера
    n = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    if (n == -1) {
        perror("recvfrom() error");
        exit(EXIT_FAILURE);
    }

    // Парикмахер разбужен
    printf("Клиент разбудил парикмахера\n");

    // Отправка времени стрижки серверу
    strcpy(buffer, argv[3]);
    n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (n == -1) {
        perror("sendto() error");
        exit(EXIT_FAILURE);
    }

    // Парикмахер стрижется
    printf("Клиент стрижется\n");

    // Ожидание сообщения от сервера о завершении стрижки
    n = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    if (n == -1) {
        perror("recvfrom() error");
        exit(EXIT_FAILURE);
    }

    // Вывод сообщения о завершении стрижки
    printf("Клиент постригся и ушел\n");

    // Закрытие сокета
    close(sockfd);

    return 0;
}
