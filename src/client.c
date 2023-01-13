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
        bzero(buffer, 256);
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
    bzero((char *)&serv_addr, sizeof(serv_addr));
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

    send_msg(sockfd, "Hello");

    close(sockfd);

    return 0;
}
