#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>

#include "error.h"
#include "interpreter.h"

int repl(void) {
  printf("Brainfuck Repl\n");
  for (;;) {
    char *prog = (char *)NULL;
    prog = readline(">> ");

    if (!strcmp(prog, "exit")) {
      break;
    } else if (!strcmp(prog, "")) {
      goto nxt_itr;
    } else if (!strcmp(prog, "reset")) {
      reset();
    } else if (strstr(prog, "print") != NULL) {
      strsep(&prog, " "); // print cmd

      char *cmd = strsep(&prog, " ");

      if (!strcmp(cmd, "idx")) {
        printf("pointer index: %d", get_pointer());
      } else if (!strcmp(cmd, "curr")) {
        printf("value at current index: %d", get_curr_ele());
      } else {
        int locs = atoi(cmd);
        for (int i = 0; i < locs; i++) {
          printf("%d: %d | ", i, get_ele_at_idx(i));
        }
      }
      printf("\n");
    } else {
      int errno = exec(prog, (int)strlen(prog));
      if (errno != 0) {
        printf("error: %s\n", strerr(errno));
      }
    }

  nxt_itr:
    free(prog);
  }

  return 0;
}
