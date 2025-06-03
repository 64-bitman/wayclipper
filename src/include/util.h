#pragma once

#define UNUSED __attribute__((unused))
#define LOG(format, ...) fprintf(stderr, format "\n", ##__VA_ARGS__)

#define FAIL -1
#define OK 0

typedef char ret_T;
