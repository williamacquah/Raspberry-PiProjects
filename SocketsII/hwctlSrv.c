#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORT 6543
#define BUF_SIZE 128

int readGPIO(const char *path) {
    char value_str[3];
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    read(fd, value_str, sizeof(value_str));
    close(fd);
    return atoi(value_str);
}

void writeGPIO(const char *path, int value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) return;
    dprintf(fd, "%d", value);
    close(fd);
}

float readTemp() {
    FILE *fp = fopen("/sys/bus/w1/devices/28-00000929e047/w1_slave", "r");
    if (!fp) return -1000;
    char line[256];
    float temp = -1000.0;
    while (fgets(line, sizeof(line), fp)) {
        char *t_ptr = strstr(line, "t=");
        if (t_ptr) {
            temp = atoi(t_ptr + 2) / 1000.0;
            break;
        }
    }
    fclose(fp);
    return temp;
}

int main() {
    int led_state = 0;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];
    socklen_t client_len = sizeof(client_addr);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Accept");
            continue;
        }

        memset(buffer, 0, BUF_SIZE);
        int bytes_received = recv(client_fd, buffer, BUF_SIZE - 1, 0);
        if (bytes_received <= 0) {
            close(client_fd);
            continue;
        }

        int option = atoi(buffer);
        int old_led = led_state;

        switch (option) {
            case 1: // φ1: Direct LED control
                led_state = !led_state;
                writeGPIO("/sys/class/gpio/gpio18/value", led_state);
                dprintf(client_fd, "%d %d\n", old_led, led_state);
                break;

            case 2: { // φ2: Conditional LED control
                int button = readGPIO("/sys/class/gpio/gpio17/value");
                if (button == 1) {
                    led_state = !led_state;
                    writeGPIO("/sys/class/gpio/gpio18/value", led_state);
                }
                dprintf(client_fd, "%d %d\n", old_led, led_state);
                break;
            }

            case 3: { // φ3: Status report
                int button = readGPIO("/sys/class/gpio/gpio17/value");
                float temp = readTemp();
                dprintf(client_fd, "%d %d %.3f\n", led_state, button, temp);
                break;
            }

            default: // Invalid input
                break;
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}