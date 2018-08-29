#ifndef __UTILS_HH__
#define __UTILS_HH__

#include <cstdbool>
#include <cstring>
#include <cassert>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>

#include <unistd.h>
#include <dlfcn.h>


/* Below OVERRIDE_* macros are to write override functions
 * for specific library symbolsn with reference to the
 * original symbol. */

#define OVERRIDE_LIBC_SYMBOL(rettype, symbol, ...) \
typedef rettype (*orig_##symbol##_ftype)(__VA_ARGS__); \
static orig_##symbol##_ftype orig_##symbol = NULL; \
extern "C" \
rettype symbol(__VA_ARGS__) { \
    do { \
        if (orig_##symbol == NULL) { \
            orig_##symbol = (orig_##symbol##_ftype) \
                    dlsym(RTLD_NEXT, #symbol); \
        } \
        assert(orig_##symbol != NULL); \
    } while (0);

const size_t MAX_KLEN = 256;
const size_t MAX_VLEN = 4096;
const size_t MAX_PATH = 260;


static inline bool has_prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}


template<typename T>
bool strtonum(const std::string& str, T& result) {
    // Specialized impls are defined in utils.cc
    static_assert(std::is_integral<T>::value, "Integral required.");
    return false;
}


/**
 * A no-op string assignment for as-is string conversion.
 */
extern bool as_is(const std::string& str, std::string& result);

extern int read_config(const char *domain, const char *key, char *value, size_t len);


template<typename K, typename V>
bool insert_kv(std::unordered_map<K, V> &target,
               const std::string &key_str,
               const std::string &value_str,
               std::function<bool(const std::string&, K&)> key_convertor,
               std::function<bool(const std::string&, V&)> value_convertor
) {
    bool success = false;
    bool key_converted = false;
    bool value_converted = false;
    K key;
    V value;
    if (key_convertor) {  // check if callable
        key_converted = key_convertor(key_str, key);
    }
    if (value_convertor) {
        value_converted = value_convertor(value_str, value);
    }
    if (key_converted && value_converted) {
        target[key] = value;
        success = true;
    }
    return success;
}


template<typename K, typename V>
size_t parse_kvpairs(
        const char *str, std::unordered_map<K, V> &kvpairs,
        std::function<bool(const std::string&, K&)> key_convertor,
        std::function<bool(const std::string&, V&)> value_convertor,
        size_t max_entries = 0,
        const char *pair_delim = ",",
        const char *kv_delim = ":")
{
    size_t num_entries = 0;
    size_t maxlen = strnlen(str, MAX_KLEN);
    char buffer[maxlen + 1] = {0};
    char pair[maxlen + 1] = {0};
    std::string key_str, value_str;

    strncpy(buffer, str, maxlen);
    char *tok = &buffer[0];

    while (tok != NULL) {
        char *end;
        bzero(pair, maxlen + 1);
        key_str = "";
        value_str = "";
        // Parse into a pair
        end = tok;
        strsep(&end, pair_delim);
        strncpy(pair, tok, maxlen);
        tok = end;
        // Parse the pair
        end = pair;
        strsep(&end, kv_delim);
        key_str = pair;
        if (end != NULL) {
            value_str = end;
        }
        // Convert and store the parsing results
        // (we append an entry only when both key/val conversion were successful)
        if (insert_kv(kvpairs, key_str, value_str,
                      key_convertor, value_convertor)) {
            num_entries ++;
            if (max_entries > 0 && num_entries == max_entries)
                break;
        }
    }
    return num_entries;
}

#endif
