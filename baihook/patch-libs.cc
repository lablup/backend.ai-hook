#include "utils.hh"
#include "patch-libs.hh"

#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cerrno>

#include <ctype.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static const char *input_sock = "/tmp/bai-user-input.sock";
static const char *input_host = "127.0.0.1";
static const int input_port = 65000;

OVERRIDE_LIBC_SYMBOL(long, sysconf, int flag)
    switch (flag) {
    case _SC_NPROCESSORS_ONLN:
    case _SC_NPROCESSORS_CONF:
        return get_num_processors_from_cpuset("/sys/fs/cgroup/cpuset/cpuset.cpus");
    /* though getpagesize() call is considered as deprecated,
     * without this, redis-server from APT package will stick on a deadlock
     * during intialization process due to their own jemalloc function loading mechanism.
     * (check 0010-Add-support-for-USE_SYSTEM_JEMALLOC-flag.patch)
     * We can remove this case if redis-server served on launchpad changes that behavior.
     */
    case _SC_PAGE_SIZE:
        return getpagesize();
    default:
        break;
    }
    LOAD_ORIG_LIBC_SYMBOL(sysconf);
    return orig_sysconf(flag);
}

int connect_input(int *fd)
{
    struct sockaddr_un un_addr;
    int sockfd;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -errno;
    }

    memset(&un_addr, 0, sizeof(un_addr));
    un_addr.sun_family = AF_UNIX;
    strcpy(un_addr.sun_path, input_sock);

    if (connect(sockfd, (struct sockaddr *) &un_addr, sizeof(un_addr)) == -1) {
        if (errno == ENOENT) { 
            goto inet;
        }
        perror("connect");
        return -errno;
    }
    *fd = sockfd;
    return 0;

inet:
    struct sockaddr_in in_addr;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -errno;
    }

    memset(&in_addr, 0, sizeof(in_addr));
    in_addr.sin_family = AF_INET;
    in_addr.sin_addr.s_addr = inet_addr(input_host);
    in_addr.sin_port = htons(input_port);

    if (connect(sockfd, (struct sockaddr *) &in_addr, sizeof(in_addr)) == -1) {
        perror("connect");
        return -errno;
    }
    *fd = sockfd;
    return 0;
}


extern "C"
int scanf(const char *format, ...)
{
    va_list args;
    char buffer[1024];
    int sockfd, err;

    err = connect_input(&sockfd);
    if (err < 0) {
        return err;
    }

    fflush(stdout);

    int recvsz = read(sockfd, buffer, 1023);
    close(sockfd);
    buffer[recvsz] = '\0';

    va_start(args, format);
    int ret = vsscanf(buffer, format, args);
    va_end(args);
    return ret;
}


extern "C"
int vscanf(const char *format, va_list args)
{
    char buffer[1024];
    int sockfd, err;

    err = connect_input(&sockfd);
    if (err < 0) {
        return err;
    }

    fflush(stdout);

    int recvsz = read(sockfd, buffer, 1023);
    close(sockfd);
    buffer[recvsz] = '\0';

    int ret = vsscanf(buffer, format, args);
    return ret;
}
