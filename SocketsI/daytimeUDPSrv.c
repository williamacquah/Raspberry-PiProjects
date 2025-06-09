#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6543
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buffer[BUFFER_SIZE];
    time_t rawtime;
    struct tm * timeinfo;
    char *response;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    while (1) {
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
        int format = atoi(buffer);

        time(&rawtime);
        if (format == 1) {
            timeinfo = localtime(&rawtime);
        } else {
            timeinfo = gmtime(&rawtime);
        }
        response = asctime(timeinfo);
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&cliaddr, len);
    }

    return 0;
}
