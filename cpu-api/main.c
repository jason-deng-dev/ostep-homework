#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void q1() {
  int x = 100;
  int rc = fork();
  if (rc < 0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) { // child
    x += 1;
    printf("child val %d\n", x);
  } else { // parent
    x += 2;
    printf("parent val %d\n", x);
  }
}

void q2() {
  int fd = open("./q2.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);

  if (fd < 0) {
    perror("open");
    exit(1);
  }

  int rc = fork();
  if (rc < 0) {
    perror("fork");
    exit(1);
  }

  else if (rc == 0) { // child
    const char *msg = "child\n";
    write(fd, msg, strlen(msg));
  } else { // parent
    const char *msg = "parent\n";
    write(fd, msg, strlen(msg));
  }
}

void q3() {
  int rc = fork();
  if (rc < 0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) {

    printf("hello\n");
  } else {
    // int rc_wait = wait(NULL);
    sleep(1);
    printf("goodbye\n");
  }
}

void q4() {
  int rc = fork();

  if (rc < 0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) {

    char *myargs[4];           // 3 args + NULL
    myargs[0] = strdup("ls");  // program name (found via $PATH)
    myargs[1] = strdup("-l");  // arg 1
    myargs[2] = strdup("/");   // arg 2
    myargs[3] = NULL;          // end marker
    execvp(myargs[0], myargs); // run it

    perror("execvp"); // only if execvp failed
    _exit(1);
  } else {
    wait(NULL);
  }
}

void q5() {
  int rc = fork();
  if (rc < 0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) {
    fprintf(stderr, "child/n");
  } else {
    int rc_wait = wait(NULL);
    fprintf(stderr, "rc_wait: %d, parent\n", rc_wait);
  }
}

void q6(void) {
  int rc = fork();
  if (rc < 0) {
    perror("fork");
    exit(1);
  }

  if (rc == 0) {
    // child1
    fprintf(stderr, "child1\n");
    _exit(0);
  }

  // parent only from here
  int rc2 = fork();
  if (rc2 < 0) {
    perror("fork");
    exit(1);
  }

  if (rc2 == 0) {
    // child2
    fprintf(stderr, "child2\n");
    _exit(0);
  }

  // parent: wait for both, or a specific one
  waitpid(rc, NULL, 0);  // wait for child1
  waitpid(rc2, NULL, 0); // wait for child2
  fprintf(stderr, "parent\n");
}

void q7() {
  int rc = fork();
  if (rc < 0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) {
    close(STDOUT_FILENO);
    printf("print after closing\n");
  } else {
    wait(NULL);
    fprintf(stderr, "parent\n");
  }
}

void q8(void) {
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(1);
  }

  /* ---------- child1: writes into the pipe ---------- */
  int rc1 = fork();
  if (rc1 < 0) {
    perror("fork1");
    exit(1);
  } else if (rc1 == 0) {
    close(pipefd[0]);               // child1 doesn't read
    dup2(pipefd[1], STDOUT_FILENO); // stdout → pipe write end
    close(pipefd[1]);               // original fd no longer needed

    printf("hello from child1\n"); // goes into the pipe
    exit(0);
  }

  /* ---------- child2: reads from the pipe ---------- */
  int rc2 = fork();
  if (rc2 < 0) {
    perror("fork2");
    exit(1);
  } else if (rc2 == 0) {
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);

    char buf[256];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
    if (n > 0) {
      buf[n] = '\0';
      printf("child2 received: %s", buf);
    }
    exit(0);
  }

  /* ---------- parent: close both ends, wait ---------- */
  close(pipefd[0]);
  close(pipefd[1]);
  wait(NULL);
  wait(NULL);
  fprintf(stderr, "parent done\n");
}

int main(int argc, char *argv[]) {
  q8();

  return 0;
}
