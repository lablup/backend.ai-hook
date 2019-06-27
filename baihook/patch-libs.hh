#ifndef __PATCH_LIBS_HH__
#define __PATCH_LIBS_HH__

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctype.h>


static int get_num_processors_from_cpuset(const char *cpuset_path)
{
    const size_t maxlen = 512;
    char line[maxlen];
    FILE *fin = fopen(cpuset_path, "r");
    if (fin == NULL)
        return 0;

    if (fgets(line, maxlen, fin) == NULL)
        return 0;

    int result = 0;
    char *l = &line[0];
    char *ends = (l + (uintptr_t) strnlen(line, maxlen));
    do {
        char *endp;
        unsigned long int n = strtoul(l, &endp, 10);
        if (l == endp) {
            result = 0;
            break;
        }
        unsigned long int m = n;
        if (*endp == '-') {
            l = endp + 1;
            m = strtoul(l, &endp, 10);
            if (l == endp) {
                result = 0;
                break;
            }
        }
        result += (m - n + 1);
        l = endp;
        while (l < ends && (isspace(*l) || *l == ','))
            ++l;
    } while (l < ends);

    fclose(fin);
    return result;
}


#endif /* __PATCH_LIBS_HH__ */
