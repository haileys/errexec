#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static int
write_in_full(int fd, const char* buff, size_t len)
{
    size_t offset = 0;

    while(offset < len) {
        ssize_t rc = write(fd, buff + offset, len - offset);

        if(rc < 0) {
            if(errno == EINTR) {
                continue;
            }

            return rc;
        }

        offset += rc;
    }

    return 0;
}

int
main(int argc, char** argv)
{
    if(argc <= 1) {
        fprintf(stderr, "usage: errexec <command> [<args>...]\n");
        exit(EXIT_FAILURE);
    }

    int stderr_pipe[2];

    if(pipe(stderr_pipe) < 0) {
        perror("errexec: pipe");
        exit(EXIT_FAILURE);
    }

    int rc = fork();

    if(rc < 0) {
        perror("errexec: fork");
        exit(EXIT_FAILURE);
    }

    if(rc == 0) {
        // child
        close(0);
        close(1);
        close(stderr_pipe[1]);

        while(1) {
            char buff[4096];

            ssize_t nread = read(stderr_pipe[0], buff, 4096);

            if(nread < 0 && errno == EINTR) {
                continue;
            }

            if(nread <= 0) {
                break;
            }

            const char* begin_red = "\033[31m";
            const char* end_red = "\033[0m";

            write_in_full(2, begin_red, strlen(begin_red));
            write_in_full(2, buff, nread);
            write_in_full(2, end_red, strlen(end_red));
        }

        exit(EXIT_SUCCESS);
    }

    dup2(stderr_pipe[1], 2);
    close(stderr_pipe[0]);
    close(stderr_pipe[1]);

    execvp(argv[1], argv + 1);

    perror("errexec");
    exit(EXIT_FAILURE);
}
