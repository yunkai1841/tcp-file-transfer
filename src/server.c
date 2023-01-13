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

int main() {
    int sockfd, new_sockfd;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    // ソケットを作成する
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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

    // SSL のコンテキストを作成する
    SSL_CTX *ctx;
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (ctx == NULL) {
        perror("ERROR creating SSL context");
        exit(1);
    }

    // 証明書を読み込む
    if (SSL_CTX_use_certificate_file(ctx, "cert/server.crt",
                                     SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    // 秘密鍵を読み込む
    if (SSL_CTX_use_PrivateKey_file(ctx, "cert/server.key", SSL_FILETYPE_PEM) <=
        0) {
        ERR_print_errors_fp(stderr);
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

    // SSL の接続を作成する
    SSL *ssl;
    ssl = SSL_new(ctx);
    if (ssl == NULL) {
        perror("ERROR creating SSL connection");
        exit(1);
    }

    // ソケットと SSL の接続を紐付ける
    if (SSL_set_fd(ssl, new_sockfd) == 0) {
        perror("ERROR linking socket and SSL connection");
        exit(1);
    }

    // SSL の接続をアクセプトする
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    // データを受信する
    bzero(buffer, 256);
    n = SSL_read(ssl, buffer, 255);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    printf("Message from client: %s\n", buffer);

    // データを送信する
    n = SSL_write(ssl, "I got your message", 18);
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    // SSL の接続をクローズする
    SSL_shutdown(ssl);
    SSL_free(ssl);

    // ソケットをクローズする
    close(new_sockfd);
    close(sockfd);

    // SSL のコンテキストを破棄する
    SSL_CTX_free(ctx);
    return 0;
}
