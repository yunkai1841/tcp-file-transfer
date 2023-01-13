#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8000

int main(int argc, char *argv[]) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    FILE *fp;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname filename\n", argv[0]);
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

    // ファイルをオープンする
    if (access(argv[2], R_OK) == -1) {
        perror("ERROR file not readable");
        exit(1);
    }
    if ((fp = fopen(argv[2], "rb")) == NULL) {
        perror("ERROR file open failed");
        exit(1);
    }

    printf("start sending file\n");

    while (1) {
        // ファイルからデータを読み込む
        bzero(buffer, 256);
        n = fread(buffer, 1, 255, fp);
        printf("read %d bytes\n", n);
        if (n < 0) {
            perror("ERROR reading from file");
            exit(1);
        }

        // データを送信する
        if (send(sockfd, buffer, n, 0) != n) {
            perror("ERROR writing to socket");
            exit(1);
        }
        printf("send\n");

        // 確認メッセージを受信する
        bzero(buffer, 256);
        if (recv(sockfd, buffer, 255, 0) < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        printf("Message from server: %s\n", buffer);

        if (n < 255) {
            break;
        }
    }
    printf("end of file\n");

    // EOFを送信して終了する
    if (send(sockfd, "EOF", 3, 0) != 3) {
        perror("ERROR writing to socket");
        exit(1);
    }

    // データを受信する
    bzero(buffer, 256);
    if (recv(sockfd, buffer, 255, 0) < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    printf("Message from server: %s\n", buffer);

    close(sockfd);
    fclose(fp);

    return 0;
}
