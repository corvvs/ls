#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len;

    // ソケットを作成
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    // sockaddr_un構造体を設定
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, "/tmp/my_unix_socket", sizeof(server_addr.sun_path) - 1);

    // ソケットにアドレスをバインド
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    // 接続要求を待機
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        close(server_fd);
        exit(1);
    }

    printf("Waiting for a connection...\n");

    // クライアントからの接続を受け入れる
    client_addr_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        exit(1);
    }

    printf("Connection accepted!\n");

    // ソケットをクローズ
    close(client_fd);
    close(server_fd);

    // ソケットファイルを削除
    unlink(server_addr.sun_path);

    return 0;
}
