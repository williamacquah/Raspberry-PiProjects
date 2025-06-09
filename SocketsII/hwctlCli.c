#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6543
#define SERVER_IP "127.0.0.1"  // Change if the server is on a different machine

int main(int argc, char *argv[]) {
    int service_number = 1;  // Default service
    if (argc >= 2) {
        service_number = atoi(argv[1]);
    }

    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    write(sock, &service_number, sizeof(int));

    if (service_number == 1 || service_number == 2) {
        int response[2];
        if (read(sock, response, sizeof(response)) > 0) {
            printf("Previous LED state: %d, Current LED state: %d\n", response[0], response[1]);
        }
    } else if (service_number == 3) {
        int response[4];
        if (read(sock, response, sizeof(response)) > 0) {
            float temperature = response[2] + (response[3] / 1000.0);
            printf("LED state: %d, Button state: %d, Temperature: %.3f°C\n",
                   response[0], response[1], temperature);
        }
    }

    close(sock);
    return 0;
}
