#include "utils.hh"

#include <cstdbool>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <string>


template<>
bool strtonum(const std::string& str, int& result) {
    int value = 0;
    char *endptr = NULL;
    errno = 0;
    value = strtol(str.c_str(), &endptr, 10);
    if (errno == ERANGE || endptr == str) {
        return false;
    }
    result = value;
    return true;
}


template<>
bool strtonum(const std::string& str, uint64_t& result) {
    uint64_t value = 0;
    char *endptr = NULL;
    errno = 0;
    value = strtoul(str.c_str(), &endptr, 10);
    if (errno == ERANGE || endptr == str) {
        return false;
    }
    result = value;
    return true;
}


template<>
bool insert_kv(std::unordered_map<std::string, std::string> &target,
               const std::string &key_str,
               const std::string &value_str,
               std::function<bool(const std::string&, std::string&)> key_convertor,
               std::function<bool(const std::string&, std::string&)> value_convertor
) {
    // For string-to-string maps, convertors are not used.
    target[key_str] = value_str;
    return true;
}


bool as_is(const std::string& str, std::string& result) {
    result = str;
    return true;
}


int read_config
(const char *domain, const char *key, char *value, size_t len)
{
    bool found = false;
    char filename[MAX_PATH] = {0};
    snprintf(filename, MAX_PATH, ".config/%s.txt", domain);
    FILE *f = fopen(filename, "r");
    if (f != NULL) {
        char buffer[MAX_KLEN + MAX_VLEN];
        while(fgets(buffer, MAX_KLEN + MAX_VLEN, f)) {
            size_t l = strnlen(buffer, MAX_KLEN + MAX_VLEN);
            if (l > 0 && buffer[l - 1] == '\n')
                buffer[l - 1] = '\0';
            char *tok = &buffer[0];
            char *end = tok;
            char line_key[MAX_KLEN];
            // split only once with "="
            strsep(&end, "=");
            strncpy(line_key, tok, MAX_KLEN);
            tok = end;
            if (strncmp(line_key, key, MAX_KLEN) == 0) {
                strncpy(value, tok, MAX_VLEN);
                found = true;
                break;
            }
        }
        fclose(f);
    }
    if (!found) {
        char env_key[256] = {0};
        snprintf(env_key, 256, "BACKEND_%s", key);
        const char *env_value = getenv(env_key);
        if (env_value == NULL) {
            return -1;
        }
        snprintf(value, len, "%s", env_value);
    }
    return 0;
}
