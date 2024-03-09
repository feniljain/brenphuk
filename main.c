#include <stdio.h>
#include <string.h>

#include "interpreter.h"
#include "repl.h"
#include "tests.h"
#include "benchmark.h"

int main(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  if (argc != 1 && argc != 2) {
    printf("brenphuk [command]: repl, tests");
  }

  reset(); // init stuff

  char *cmd = argv[1];
  if (cmd == NULL || !strcmp(cmd, "repl")) {
    return repl();
  } else if (!strcmp(cmd, "tests")) {
    tests();
  } else if (!strcmp(cmd, "benchmark")) {
    benchmark();
  }

  return 0;
}
