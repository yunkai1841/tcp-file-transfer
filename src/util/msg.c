#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "error.h"

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

void send_msg(int sockfd, char* msg) {
    int n;
    char buffer[256];

    // データを送信する
    n = send(sockfd, msg, strlen(msg), 0);
    if (n < 0) {
        exit_with_msg("ERROR writing to socket");
    }
    DEBUG_PRINT("send %d bytes\n", n);

    // 確認メッセージを受信する
    memset(buffer, 0, 256);
    n = recv(sockfd, buffer, 255, 0);
    if (n < 0) {
        exit_with_msg("ERROR reading from socket");
    }
    DEBUG_PRINT("Message from server: %s\n", buffer);
}

void receive_msg(int sockfd, char* buffer, int size) {
    int n;

    // データを受信する
    memset(buffer, 0, size);
    n = recv(sockfd, buffer, size - 1, 0);
    if (n < 0) {
        exit_with_msg("ERROR reading from socket");
    }
    DEBUG_PRINT("Message from server: %s\n", buffer);

    // 確認メッセージを送信する
    n = send(sockfd, "OK", 2, 0);
    if (n < 0) {
        exit_with_msg("ERROR writing to socket");
    }
    DEBUG_PRINT("send %d bytes\n", n);
}