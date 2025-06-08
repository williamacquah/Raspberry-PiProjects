#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6543
#define BUF_SIZE 128

int main(int argc, char *argv[]) {
    int option = 1;
    if (argc >= 2) option = atoi(argv[1]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("Socket"); exit(1); }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = inet_addr("127.0.0.1")  // Use localhost if client runs on same Pi
    };

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect"); exit(1);
    }

    char send_buf[BUF_SIZE], recv_buf[BUF_SIZE];
    snprintf(send_buf, BUF_SIZE, "%d", option);
    send(sock, send_buf, strlen(send_buf), 0);

    memset(recv_buf, 0, BUF_SIZE);
    recv(sock, recv_buf, BUF_SIZE, 0);
    printf("Server Response: %s\n", recv_buf);

    close(sock);
    return 0;
}
