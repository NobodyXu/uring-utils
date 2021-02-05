#define _XOPEN_SOURCE 500 // Fix sigset_t not found error in liburing.h
#define _GNU_SOURCE // For loff_t
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <liburing.h>

static const int out_fd = 1;

static bool is_pipe(int fd)
{
    struct stat statbuf;

    if (fstat(fd, &statbuf) < 0)
        err(1, "fstat %d failed", fd);

    return S_ISFIFO(statbuf.st_mode);
}

static struct io_uring_cqe* io_uring_get_first_cqe(struct io_uring *ring)
{
	struct io_uring_cq *cq = &ring->cq;
    return &cq->cqes[(*cq->khead) & (*cq->kring_mask)];
}

static bool io_uring_has_cqe(struct io_uring *ring)
{
	struct io_uring_cq *cq = &ring->cq;
    return *cq->khead != io_uring_smp_load_acquire(ring->cq.ktail);
}

/**
 * in_fd or out_fd is a pipe, so use spice directly
 */
static int splice1(struct io_uring *ring, int in_fd, unsigned len)
{
    for (struct io_uring_sqe *sqe; (sqe = io_uring_get_sqe(ring)); ) {
        io_uring_prep_splice(sqe, in_fd, (loff_t) -1, out_fd, (loff_t) -1, len, 0);

		int ret = io_uring_submit_and_wait(ring, 1);
		if (ret < 0) {
			fprintf(stderr, "sqe submit failed: %s\n", strerror(-ret));
			return ret;
		}

        struct io_uring_cqe *cqe = io_uring_get_first_cqe(ring);
        ret = cqe->res;
        io_uring_cqe_seen(ring, cqe);

        if (ret < 0) {
            fprintf(stderr, "splice on line %d failed: %s\n", __LINE__, strerror(-ret));
            return 1;
        } else if (ret == 0)
            return 0;
    }

    fputs("io_uring_get_sqe returned NULL", stderr);
    return 1;
}

/**
 * Neither in_fd nor out_fd is a pipe, so create one and splice.
 */
int splice2(struct io_uring *ring, int in_fd, unsigned len)
{
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        warn("pipe failed");
        return 1;
    }

    bool in_fd_consumed = false;

    for (struct io_uring_sqe *sqe; (sqe = io_uring_get_sqe(ring)); ) {
        if (!in_fd_consumed) {
            io_uring_prep_splice(sqe, in_fd, (loff_t) -1, pipefd[1], (loff_t) -1, len, 0);
            sqe->flags |= IOSQE_ASYNC;
            sqe->user_data = 0;

            sqe = io_uring_get_sqe(ring);
            if (!sqe)
                break;
        }

        io_uring_prep_splice(sqe, pipefd[0], (loff_t) -1, out_fd, (loff_t) -1, len, 0);
        sqe->flags |= IOSQE_ASYNC;
        sqe->user_data = 1;

		int ret = io_uring_submit_and_wait(ring, in_fd_consumed ? 1 : 2);
		if (ret < 0) {
            fprintf(stderr, "sqe submit failed: %s\n", strerror(-ret));
            return ret;
        }

        do {
            struct io_uring_cqe *cqe = io_uring_get_first_cqe(ring);
            ret = cqe->res;
            uint64_t data = cqe->user_data;
            io_uring_cqe_seen(ring, cqe);

            if (ret < 0) {
                fprintf(stderr, "splice which has data %zu on line %d failed: %s\n",
                        (size_t) data, __LINE__, strerror(-ret));
                return 1;
            } else if (ret == 0) {
                if (data == 0) {
                    in_fd_consumed = true;

                    sqe = io_uring_get_sqe(ring);
                    if (sqe == NULL)
                        break;
                    io_uring_prep_close(sqe, pipefd[1]);
                    sqe->user_data = 3;
                } else if (data == 1)
                    return 0;
            }
        } while (io_uring_has_cqe(ring));
    }

    fputs("io_uring_get_sqe returned NULL", stderr);
    return 1;
}

int main(int argc, char* argv[])
{
    int in_fd = 0;

    if (argc == 2) {
        in_fd = open(argv[1], O_RDONLY);
        if (in_fd < 0)
            err(1, "failed to open %s", argv[1]);
    }

    struct io_uring ring;
    int errno_v = io_uring_queue_init(16, &ring, 0);
    if (errno_v != 0) {
		fprintf(stderr, "io_uring_setup failed: %s\n", strerror(-errno_v));
        return 1;
    }

    int exit_status;

    bool is_in_fd_pipe = is_pipe(in_fd);
    bool is_out_fd_pipe = is_pipe(out_fd);

    if (is_in_fd_pipe || is_out_fd_pipe)
        exit_status = splice1(&ring, in_fd, 1024);
    else
        exit_status = splice2(&ring, in_fd, 1024);

    io_uring_queue_exit(&ring);

    return exit_status;
}
