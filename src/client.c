#include <netdb.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
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
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname message\n", argv[0]);
        exit(0);
    }

    // ソケットを作成する
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // SSL のコンテキストを作成する
    SSL_CTX *ctx;
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL) {
        perror("ERROR creating SSL context");
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

    // SSL の接続を作成する
    SSL *ssl;
    ssl = SSL_new(ctx);
    if (ssl == NULL) {
        perror("ERROR creating SSL connection");
        exit(1);
    }

    // ソケットと SSL の接続を紐付ける
    if (SSL_set_fd(ssl, sockfd) == 0) {
        perror("ERROR linking socket and SSL connection");
        exit(1);
    }

    // SSL の接続を開始する
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    // データを送信する
    bzero(buffer, 256);
    strcpy(buffer, argv[2]);
    n = SSL_write(ssl, buffer, strlen(buffer));
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    // データを受信する
    bzero(buffer, 256);
    n = SSL_read(ssl, buffer, 255);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    printf("Message from server: %s\n", buffer);

    // SSL の接続をクローズする
    SSL_shutdown(ssl);
    SSL_free(ssl);

    // ソケットをクローズする
    close(sockfd);

    // SSL のコンテキストを破棄する
    SSL_CTX_free(ctx);

    return 0;
}
