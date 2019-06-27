#include "gtest/gtest.h"

#include <sys/types.h>
#include <unistd.h>
#include <sched.h>

namespace {

TEST(sysconf, hooked) {
    int ret;
    int pid = getpid();
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    EXPECT_EQ(0, CPU_COUNT(&cpuset));
    ret = sched_getaffinity(pid, sizeof(cpuset), &cpuset);
    EXPECT_EQ(ret, 0);

    // check cpuset options given to the docker command in test.sh
    EXPECT_EQ(4, CPU_COUNT(&cpuset));
    EXPECT_TRUE(CPU_ISSET(0, &cpuset));
    EXPECT_TRUE(CPU_ISSET(1, &cpuset));
    EXPECT_TRUE(CPU_ISSET(4, &cpuset));
    EXPECT_TRUE(CPU_ISSET(5, &cpuset));

    // compare results of sysconf and cpuset
    int ncpu_sysconf = sysconf(_SC_NPROCESSORS_ONLN);
    EXPECT_NE(0, ncpu_sysconf);
    int ncpu_cpuset = CPU_COUNT(&cpuset);
    EXPECT_NE(0, ncpu_cpuset);
    EXPECT_EQ(ncpu_sysconf, ncpu_cpuset);
}

}
