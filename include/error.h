#ifndef ERR_H
#define ERR_H
#include <debug.h>

// error handling
enum Error { ERR_NO_MATCHING_BRACKET = 1 };

char *strerr(int errnum) {
  switch (errnum) {
  case ERR_NO_MATCHING_BRACKET:
    return "did not find matching loop bracket";
  default:
    ABORT("incorrect error num passed");
  }
}

#endif
