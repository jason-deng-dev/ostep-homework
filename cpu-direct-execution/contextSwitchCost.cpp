#define _GNU_SOURCE
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>

void pin(int cpu) {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    if (sched_setaffinity(0, sizeof(set), &set) != 0) { perror("pin"); exit(1); }
}

int main() {
    const int N = 100000;
    int p1[2], p2[2];
    pipe(p1); pipe(p2);

    if (fork() == 0) {
        pin(0);
        close(p1[1]); close(p2[0]);
        char c;
        for (int i = 0; i < N; ++i) {
            read(p1[0], &c, 1);
            write(p2[1], &c, 1);
        }
        _exit(0);
    }

    pin(0);
    close(p1[0]); close(p2[1]);
    char c = 'x';

    auto t1 = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) {
        write(p1[1], &c, 1);
        read(p2[0], &c, 1);
    }
    auto t2 = std::chrono::steady_clock::now();

    double ns = std::chrono::duration<double, std::nano>(t2 - t1).count();
    printf("round trip: %.2f ns  |  per ctx switch: %.2f ns\n", ns / N, ns / N / 2);
    wait(nullptr);
}
