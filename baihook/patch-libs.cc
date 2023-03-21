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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

static const char *input_host = "127.0.0.1";
static const int input_port = 65000;


OVERRIDE_LIBC_SYMBOL(long, sysconf, int flag)
    switch (flag) {
    case _SC_NPROCESSORS_ONLN:  /* 84 */
    case _SC_NPROCESSORS_CONF:  /* 83 */
        int result;

        /* check cgroup v2 cpuset controller */
        result = get_num_processors_from_cpuset("/sys/fs/cgroup/cpuset.cpus.effective");              
        if (result == 0) {  /* fallback to cgroup v2*/
            errno = 0;  /* not clearing errno set by previous get_num~ call can cause side effect */
            result = get_num_processors_from_cpuset("/sys/fs/cgroup/cpuset/cpuset.effective_cpus");
        }
        return result;

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


extern "C"
int scanf(const char *format, ...)
{
    va_list args;
    char buffer[1024];
    struct sockaddr_in addr;
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -errno;
    }

    fflush(stdout);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(input_host);
    addr.sin_port = htons(input_port);

    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("connect");
        return -errno;
    }

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
    struct sockaddr_in addr;
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -errno;
    }

    fflush(stdout);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(input_host);
    addr.sin_port = htons(input_port);

    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("connect");
        return -errno;
    }

    int recvsz = read(sockfd, buffer, 1023);
    close(sockfd);
    buffer[recvsz] = '\0';

    int ret = vsscanf(buffer, format, args);
    return ret;
}
