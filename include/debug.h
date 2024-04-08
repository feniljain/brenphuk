#ifndef DBG_H

#define DBG_H
#include <stdlib.h>
#include <stdio.h>

#define DBG_LOGS // comment this to disable debug comments

#ifdef DBG_LOGS
#define DBG_PRINT(l) fprintf(stderr, "DEBUG: %s\n", l);
#define DBG_PRINTF(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", __VA_ARGS__);
#else
#define DBG_PRINT(l)                                                           \
  do {                                                                         \
  } while (0)
#define DBG_PRINTF(fmt, ...)                                                   \
  do {                                                                         \
  } while (0)
#endif

// #define assert(x) ((x) ? (void)0 : (printf("Assertion for %s @ %s:%d!\n", #x,
// __FILE__, __LINE__), abort()))

#define ABORT(l)                                                               \
  fprintf(stderr, "%s\n", l);                                                  \
  abort();

#define UNUSED(expr) do { (void)(expr); } while (0)

#endif
