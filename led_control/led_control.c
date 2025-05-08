#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// Utility function to write to sysfs files
void writeToFile(const char* path, const char* value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("Error opening file");
        return;
    }
    write(fd, value, strlen(value));
    close(fd);
}

int main() {
    // Step 1: Export GPIO 18
    writeToFile("/sys/class/gpio/export", "18");
    usleep(100000); // wait 100 ms

    // Step 2: Set direction to output
    writeToFile("/sys/class/gpio/gpio18/direction", "out");

    // Step 3: Turn LED on
    writeToFile("/sys/class/gpio/gpio18/value", "1");

    // Step 4: Wait 1 second
    sleep(1);

    // Step 5: Turn LED off
    writeToFile("/sys/class/gpio/gpio18/value", "0");

    return 0;
}
