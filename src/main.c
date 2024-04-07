#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "repl.h"
#include "tests.h"
#include "benchmark.h"
#include "debug.h"

int main(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  if (argc != 1 && argc != 2) {
    printf("brenphuk [command]: repl, tests, benchmark <iterations>\n");
  }

  reset(); // init stuff

  if (argv[1] == NULL || !strcmp(argv[1], "repl")) {
    return repl();
  } else if (!strcmp(argv[1], "tests")) {
    tests();
  } else if (!strcmp(argv[1], "benchmark")) {
		int itrs = 10;

		if(argc == 3) {
      itrs = atoi(argv[2]);
		}

    benchmark(itrs);
  }

  return 0;
}
