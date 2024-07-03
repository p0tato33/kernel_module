#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define PROCFS_PATH "/proc/secret"
#define BUFFER_SIZE 1024

void write_secret(const char *secret) {
    int fd = open(PROCFS_PATH, O_WRONLY);
    if (fd < 0) {
        perror("open");
        return;
    }
    write(fd, secret, strlen(secret));
    close(fd);
}

void read_secret() {
    char buffer[BUFFER_SIZE];
    int fd = open(PROCFS_PATH, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }
    int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read");
        close(fd);
        return;
    }
    buffer[bytes_read] = '\0';
    printf("Secret: %s\n", buffer);
    close(fd);
}

void delete_secret() {
    write_secret("DELETE");
}

int main() {
    printf("Записываю секрет\n");
    write_secret("Секретный текст");
    printf("Читаю секрет\n");
    read_secret();
    printf("Удаляю секрет\n");
    delete_secret();
    printf("Читаю секрет\n");
    read_secret();
    return 0;
}
