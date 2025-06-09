#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6543
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUFFER_SIZE];
    char input[2];
    socklen_t len = sizeof(servaddr);

    printf("Enter 1 for local time, any other number for GMT: ");
    fgets(input, sizeof(input), stdin);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    sendto(sockfd, input, strlen(input), 0, (const struct sockaddr *)&servaddr, len);
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    printf("Server time: %s\n", buffer);

    close(sockfd);
    return 0;
}
