#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

#define BACKLOG 10

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("\n");
    printf("=============\n");
    printf(" Запуск лога \n");
    printf("=============\n\n");

    // Создание сокета
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    // Подключение к серверу
    struct sockaddr_in servaddr;
    socklen_t servaddr_len = sizeof(servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    char buffer[1024];

    // Отправка сообщения серверу
    int n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &servaddr, servaddr_len);
    if (n == -1) {
        perror("sendto() error");
        exit(EXIT_FAILURE);
    }

    while (1) {
        struct pollfd pfd;
        pfd.fd = sockfd;
        pfd.events = POLLIN;
        poll(&pfd, 1, -1);
        char buffer1[1024];
        n = recvfrom(sockfd, buffer1, sizeof(buffer1), 0, (struct sockaddr *) &servaddr, &servaddr_len);
        if (n == -1) {
            perror("recvfrom() error");
            exit(EXIT_FAILURE);
        }

        if (buffer1[0] != '*') {
            printf("%s", buffer1);
        }
        poll(&pfd, 1, 1);
    }

    return 0;
}
