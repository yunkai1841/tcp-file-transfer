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

#define PORT 8000
#define SERVER_DIR "dir"

void send_file(int sockfd, char *filename) {
    FILE *fp;
    char buffer[256];
    int n;

    // ファイルをオープンする
    if (access(filename, R_OK) == -1) {
        exit_with_msg("ERROR file not readable");
    }
    if ((fp = fopen(filename, "rb")) == NULL) {
        exit_with_msg("ERROR file open failed");
    }

    printf("start sending file\n");
    while (1) {
        // ファイルからデータを読み込む
        memset(buffer, 0, 256);
        n = fread(buffer, 1, 255, fp);
        printf("read %d bytes\n", n);
        if (n < 0) {
            exit_with_msg("ERROR reading from file");
        }

        // データを送信する
        if (send(sockfd, buffer, n, 0) != n) {
            exit_with_msg("ERROR writing to socket");
        }
        printf("send\n");

        // 確認メッセージを受信する
        memset(buffer, 0, 256);
        if (recv(sockfd, buffer, 255, 0) < 0) {
            exit_with_msg("ERROR reading from socket");
        }
        printf("Message from server: %s\n", buffer);

        if (n < 255) {
            break;
        }
    }
    fclose(fp);
    printf("end sending file\n");
}

void send_filelist(int sockfd, char *dirname) {
    char file_list[MAX_FILES][NAME_MAX + 1];
    int file_count = filelist(dirname, file_list);
    for (int i = 0; i < file_count; i++) {
        printf("%s\n", file_list[i]);
    }

    // ファイル数を送信する
    char buffer[256];
    sprintf(buffer, "%d", file_count);
    send_msg(sockfd, buffer);

    // ファイル名を送信する
    for (int i = 0; i < file_count; i++) {
        send_msg(sockfd, file_list[i]);
    }
}

int main() {
    int sockfd, new_sockfd;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    FILE *fp;

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

    // クライアントからの接続を待つ
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    new_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (new_sockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }
    printf("connected\n");

    while (1) {
        char command[256];
        char filename[256];

        // コマンドを受信する
        receive_msg(new_sockfd, command, 256);
        printf("command: %s\n", command);

        if (strcmp(command, "ls") == 0) {
            // ファイルリストを送信する
            send_filelist(new_sockfd, SERVER_DIR);
        } else if (strcmp(command, "get") == 0) {
            // ファイル名を受信する
            receive_msg(new_sockfd, filename, 256);
            printf("filename: %s\n", filename);

            char path[256];
            sprintf(path, "%s/%s", SERVER_DIR, filename);

            // ファイルを送信する
            send_file(new_sockfd, path);
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            exit_with_msg("ERROR unknown command");
        }
    }

    close(new_sockfd);
    close(sockfd);

    return 0;
}
