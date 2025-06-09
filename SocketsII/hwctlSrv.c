#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6543
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int led_state = 0;
    int button_state = 1;  // Example button state (can be simulated)
    float temperature = 23.546; // Example temperature

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port 6543
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        int service_number;
        if (read(new_socket, &service_number, sizeof(int)) <= 0) {
            perror("Read failed");
            close(new_socket);
            continue;
        }

        printf("Received service number: %d\n", service_number);

        switch (service_number) {
            case 1: {
                int prev_state = led_state;
                led_state = !led_state;
                int response[2] = {prev_state, led_state};
                write(new_socket, response, sizeof(response));
                break;
            }
            case 2: {
                int prev_state = led_state;
                if (button_state) {
                    led_state = !led_state;
                }
                int response[2] = {prev_state, led_state};
                write(new_socket, response, sizeof(response));
                break;
            }
            case 3: {
                int states[2] = {led_state, button_state};
                int temp_int = (int)temperature;
                int temp_frac = (int)((temperature - temp_int) * 1000); // 3 decimal precision
                int response[4] = {states[0], states[1], temp_int, temp_frac};
                write(new_socket, response, sizeof(response));
                break;
            }
            default: {
                printf("Invalid service number received: %d\n", service_number);
                close(new_socket);
                continue;
            }
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}
