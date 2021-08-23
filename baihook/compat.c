#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void *__wrap_memcpy(void *dest, const void *src, size_t n)
{
    return memcpy(dest, src, n);
}

#ifdef __cplusplus
}
#endif
