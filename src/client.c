#include <netdb.h>
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

void receive_file(int sockfd, char *filename) {
    FILE *fp;
    char buffer[256];
    int n;

    // ファイルをオープンする
    if ((fp = fopen(filename, "wb")) == NULL) {
        exit_with_msg("ERROR file open failed");
    }

    printf("start receiving file\n");
    while (1) {
        // データを受信する
        memset(buffer, 0, 256);
        n = recv(sockfd, buffer, 255, 0);
        if (n == -1) {
            perror("ERROR reading from socket");
            exit(1);
        }
        printf("received %d bytes\n", n);

        // ファイルに書き込む
        if (fwrite(buffer, sizeof(char), n, fp) < n) {
            perror("ERROR file write");
            exit(1);
        }
        printf("wrote %d bytes\n", n);

        // 確認メッセージを送信する
        if (send(sockfd, "SUCCESS", 18, 0) < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }

        if (n < 255) {
            break;
        }
    }
    fclose(fp);
    printf("end receiving file\n");
}

void receive_filelist(int sockfd) {
    char file_list[MAX_FILES][NAME_MAX + 1];
    int file_count;
    char buffer[256];
    int i;

    // ファイル数を受信する
    memset(buffer, 0, 256);
    receive_msg(sockfd, buffer, 256);

    file_count = atoi(buffer);
    printf("file count: %d\n", file_count);

    // ファイル名を受信する
    for (i = 0; i < file_count; i++) {
        receive_msg(sockfd, file_list[i], NAME_MAX + 1);
        printf("%s\n", file_list[i]);
    }
}

int main(int argc, char *argv[]) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    FILE *fp;
    char buffer[256];

    if (argc < 2) {
        fprintf(stderr, "usage %s hostname\n", argv[0]);
        exit(0);
    }

    // ソケットを作成する
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // サーバーのホスト名を取得する
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    // 接続先アドレスを設定する
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(PORT);

    // サーバーに接続する
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    printf("connected\n");

    while (1) {
        char command[256];
        char filename[256];

        printf("command: ");
        scanf("%s", command);

        if (strcmp(command, "ls") == 0) {
            // ファイル一覧を取得する
            send_msg(sockfd, "ls");
            receive_filelist(sockfd);
        } else if (strcmp(command, "get") == 0) {
            send_msg(sockfd, "get");
            // ファイルを取得する
            printf("filename: ");
            scanf("%s", filename);
            send_msg(sockfd, filename);
            receive_file(sockfd, filename);
        } else if (strcmp(command, "exit") == 0) {
            send_msg(sockfd, "exit");
            break;
        } else {
            printf("unknown command: %s\n", command);
        }
    }

    close(sockfd);

    return 0;
}
