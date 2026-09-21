#define _GNU_SOURCE
#include <chrono>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/null", O_RDONLY);
    const int N = 1000000;
    char buf[1];

    auto t1 = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) read(fd, buf, 0);
    auto t2 = std::chrono::steady_clock::now();

    double ns = std::chrono::duration<double, std::nano>(t2 - t1).count();
    printf("read: %.2f ns/call\n", ns / N);
    close(fd);
}

