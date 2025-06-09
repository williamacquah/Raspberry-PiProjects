#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6543
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int opt = 1;
    char buffer[BUFFER_SIZE] = {0};
    time_t rawtime;
    struct tm * timeinfo;
    char *response;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        read(new_socket, buffer, BUFFER_SIZE);
        int format = atoi(buffer);

        time(&rawtime);
        if (format == 1) {
            timeinfo = localtime(&rawtime);
        } else {
            timeinfo = gmtime(&rawtime);
        }
        response = asctime(timeinfo);
        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }

    return 0;
}
