#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8000
#define FILENAME "received.jpg"

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
    bzero((char *)&serv_addr, sizeof(serv_addr));
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

    // ファイルをオープンする
    if ((fp = fopen(FILENAME, "wb")) == NULL) {
        perror("ERROR file open");
        exit(1);
    }

    printf("start receiving file\n");

    while (1) {
        // データを受信する
        bzero(buffer, 256);
        n = recv(new_sockfd, buffer, 255, 0);
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
        if (send(new_sockfd, "SUCCESS", 18, 0) < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }

        if (n < 255) {
            break;
        }
    }
    printf("end receiving file\n");

    // データを受信する
    bzero(buffer, 256);
    if (recv(new_sockfd, buffer, 255, 0) < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    printf("Message from client: %s", buffer);

    // データを送信する
    if (send(new_sockfd, "SUCCESS", 18, 0) < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    close(new_sockfd);
    close(sockfd);
    fclose(fp);

    return 0;
}
