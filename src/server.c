#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "util/error.h"
#include "util/filelist.h"
#include "util/msg.h"

#define DEFAULT_PORT 8080

void send_http_response(int sockfd, char *filename) {
    char buffer[256];
    sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    send_msg(sockfd, buffer);
    send_file(sockfd, filename);
}

void send_file(int sockfd, char *filename) {
    FILE *fp;
    char buffer[2048];
    int n;

    // ファイルをオープンする
    if (access(filename, R_OK) == -1) {
        exit_with_msg("ERROR file not readable");
    }
    if ((fp = fopen(filename, "rb")) == NULL) {
        exit_with_msg("ERROR file open failed");
    }

    // ファイルのサイズを取得する
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // ファイルの内容を送信する
    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        send(sockfd, buffer, n, 0);
    }

    fclose(fp);
    printf("end sending file\n");
}

int main(int argc, char *argv[]) {
    int sockfd, new_sockfd;
    socklen_t clilen;
    char buffer[10000];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    FILE *fp;
    // port番号を指定する
    int PORT = DEFAULT_PORT;
    if (argc > 1) {
        PORT = atoi(argv[1]);
    }

    // ソケットを作成する
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // ソケットにアドレスを割り当てる
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // webサーバーの処理
    while (1) {
        // クライアントからの接続を待つ
        listen(sockfd, 5);
        clilen = sizeof(cli_addr);
        new_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (new_sockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }
        printf("connected\n");

        // GETリクエストを受信する
        memset(buffer, 0, sizeof(buffer));
        n = recv(new_sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        printf("%s\n", buffer);
        sleep(1);

        // HTMLファイルを送信する
        if (strncmp(buffer, "GET / ", 6) == 0) {
            printf("send index.html\n");
            send_http_response(new_sockfd, "index.html");
        } else if (strncmp(buffer, "GET /dir/", 9) == 0) {
            char filename[NAME_MAX + 1];
            // 9文字目から"HTTP/" まで
            int request_header_end = strstr(buffer + 9, " HTTP/1.1") - buffer;
            printf("request_header_end: %d\n", request_header_end);
            strncpy(filename, buffer + 4, request_header_end - 4);
            filename[request_header_end - 4] = '\0';
            char path[NAME_MAX + 1];
            sprintf(path, ".%s", filename);
            printf("send %s\n", path);
            send_http_response(new_sockfd, path);
        } else {
            printf("send 404.html\n");
            send_http_response(new_sockfd, "404.html");
        }
    }

    close(new_sockfd);
    close(sockfd);

    return 0;
}
