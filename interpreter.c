#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <readline/readline.h>

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
  fprintf(stderr, "unrecognized error: %s\n", l);                              \
  abort();

// ======================================

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

// ======================================

const int STACK_SIZE = 1000; // 1000 nested for loops possible at max
const int TAPE_SIZE = 30000;

int main(int argc, char *argv[]);

// interpreter impl

int pointer;
char tape[TAPE_SIZE];
int brackets_loc[TAPE_SIZE][2];

void print_bracket_arr(int stop_len) {
  int i = 0;

  while (true) {
    if (stop_len != -1) {
      if (i > stop_len) {
        break;
      }
    } else {
      if (brackets_loc[i][0] == -1) {
        break;
      }
    }

    DBG_PRINTF("brackets_loc[%d]: [: %d ]: %d", i, brackets_loc[i][0],
               brackets_loc[i][1]);
    i += 1;
  }
}

// could I use a hashmap here? Well for small programs and small
// number of brackets, a linear search will be faster (tsoding ftw)
void fill_brackets_loc(char *prog, int prog_len) {
  int i = 0, idx = 0;
  while (i < prog_len) {
    switch (prog[i]) {
    case '[': {
      brackets_loc[idx][0] = i;

      int brackets_depth = 0;
      for (int j = i; j < prog_len; j++) {
        if (prog[j] == '[') {
          brackets_depth++;
        } else if (prog[j] == ']') {
          brackets_depth--;
        }

        if (brackets_depth == 0) {
          brackets_loc[idx][1] = j;
          break;
        }
      }

      if (brackets_depth != 0) {
        ABORT("brackets mismatch");
      }

      idx++;
      break;
    }
    default:
      break;
    }

    i++;
  }
}

enum Bracket { OPEN = 1, CLOSE = 2 };

int get_matching_bracket_idx(int idx, int br) {
  int check_idx = -1, ret_idx = -1;
  switch (br) {
  case OPEN:
    check_idx = 0;
    ret_idx = 1;
    break;
  case CLOSE:
    check_idx = 1;
    ret_idx = 0;
    break;
  default:
    ABORT("this is an internal function, what you passing dumbo");
    break;
  }

  for (int i = 0; brackets_loc[i][0] != -1; i++) {
    if (idx == brackets_loc[i][check_idx]) {
      return brackets_loc[i][ret_idx];
    }
  }

  return -1;
}

double benchmark_results[8][2]; // 7 commands, 1st column for count, 2nd for
                                // exec avg time

void print_benchmark_results(void) {
  for (int i = 0; i < 8; i++) {
    DBG_PRINTF("%d command executed %f times with %f total exec time", i,
               benchmark_results[i][0], benchmark_results[i][1]);
  }
}

// TODO(feniljain): shift all operations to functions and try flamegraph
int exec(char *prog, int prog_len) {
  DBG_PRINT(prog);
  int i = 0, b_idx; // , cnt = 0;

  // clock_t start, end;
  // double cpu_time_used;

  fill_brackets_loc(prog, prog_len);
  print_bracket_arr(-1);

  while (i < prog_len && i >= 0) {
    // int dataptr = pointer;
    // cnt++;
    // start = clock();
    switch (prog[i]) {
    case '>':
      b_idx = 0;
      pointer++;
      break; // TODO(feniljain): Add bound checks here
    case '<':
      b_idx = 1;
      pointer--;
      break; // TODO(feniljain): Add bound checks here
    case '+':
      b_idx = 2;
      tape[pointer]++;
      break; // I don't care about int overflow here
    case '-':
      b_idx = 3;
      tape[pointer]--;
      break;
    case '.':
      b_idx = 4;
      printf("%c", tape[pointer]);
      break;
    case ',': {
      b_idx = 5;
      char ch;
      scanf("%c", &ch);
      tape[pointer] = ch;
      break;
    }
    case '[':
    case ']': {
      int br = OPEN;
      bool condition;
      if (prog[i] == '[') {
        b_idx = 6;
        condition = tape[pointer] == 0;
      } else {
        b_idx = 7;
        condition = tape[pointer] != 0;
        br = CLOSE;
      }

      if (condition) {
        int matching_idx = get_matching_bracket_idx(i, br);
        if (matching_idx == -1) {
          ABORT("could not find matching bracket, bug in bracket finding code");
        }
        // DBG_PRINTF("] i: %d", i);
        i = matching_idx;
        continue;
      }

      break;
    }
    default:
      break;
    }

    i++;

    (void)b_idx;

    // end = clock();
    // cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    // benchmark_results[b_idx][0]++;
    // benchmark_results[b_idx][1] += cpu_time_used;
    // if (cnt == 200000000) {
    //   break;
    // }
  }

  // print_benchmark_results();
  return 0;
}

void reset(void) {
  memset(tape, 0, sizeof tape);
  pointer = 0;

  memset(brackets_loc, -1, sizeof brackets_loc);
}

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
        printf("pointer index: %d", pointer);
      } else if (!strcmp(cmd, "curr")) {
        printf("value at current index: %d", tape[pointer]);
      } else {
        int locs = atoi(cmd);
        for (int i = 0; i < locs; i++) {
          printf("%d: %d | ", i, tape[i]);
        }
      }
      printf("\n");
    } else {
      int errno = exec(prog, (int)strlen(prog));
      if (errno != 0) {
        printf("error: %s\n", strerr(errno));
      }
    }

  // add_history(); // TODO(feniljain): remember to check if it's an empty line,
  // don't add it
  nxt_itr:
    free(prog);
  }

  return 0;
}

int tests(void) {
  exec("+++", 3);
  assert(tape[0] == 3);

  reset();
  DBG_PRINT("tests::test 1 done");

  // opening bracket for loop test
  exec("[++]", 4);
  assert(tape[0] == 0);

  reset();
  DBG_PRINT("tests::test 2 done");

  // nested opening bracket for loop test
  exec("[++[++]]", 8);
  assert(tape[0] == 0);

  reset();
  DBG_PRINT("tests::test 3 done");

  // closing bracket for loop test
  exec("+++[-]", 6);
  assert(tape[0] == 0);

  reset();
  DBG_PRINT("tests::test 4 done");

  exec(">+++++++++[<++++++>-]<...>++++++++++.", 37);
  assert(tape[0] == 54);
  assert(tape[1] == 10);

  reset();
  DBG_PRINT("tests::test 5 done");

  // part of hello world
  exec("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]", 49);
  assert(tape[0] == 0);
  assert(tape[1] == 0);
  assert(tape[2] == 72);
  assert(tape[3] == 104);
  assert(tape[4] == 88);
  assert(tape[5] == 32);
  assert(tape[6] == 8);

  reset();
  DBG_PRINT("tests::test 6 done");

  // // hello world
  exec("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>"
       ">.<-.<.+++.------.--------.>>+.>++.",
       106);

  reset();
  DBG_PRINT("tests::test 7 done");

  return 0;
}

typedef struct {
  char *prog;
  int len;
} ProgStr;

ProgStr read_file(char *file_path) {
  FILE *fptr;

  // char cwd[1000];
  // getcwd(&cwd[0], 1000);
  // DBG_PRINTF("cwd: %s", cwd);

  int idx = 0;
  char *buf = NULL, *prog = (char *)malloc(1000000);
  assert(prog != NULL);

  size_t linecap = 0;
  ssize_t linelen;

  fptr = fopen(file_path, "r");
  assert(fptr != NULL);

  while ((linelen = getline(&buf, &linecap, fptr)) > 0) {
    // converting linelen is safe cause we have checked it's greater than zero
    // already
    memmove(prog + idx, buf, (size_t)linelen);
    idx += linelen;
  }

  ProgStr prog_str;
  prog_str.prog = prog;
  prog_str.len = idx;

  return prog_str;
}

int benchmark(void) {
  clock_t start, end;
  double cpu_time_used;

  ProgStr prog_str = read_file("../programs/mandlebrot.bf");
  DBG_PRINTF("prog: %s: %d", prog_str.prog, prog_str.len);

  start = clock();

  exec(prog_str.prog, prog_str.len);

  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  DBG_PRINTF("benchmark::program 1 took: %f secs", cpu_time_used);

  reset();

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 1 && argc != 2) {
    printf("brenphuk [command]: repl, tests");
  }

  memset(tape, 0, sizeof tape); // Make sure all tape is set to zero
  pointer = 0;

  memset(brackets_loc, -1, sizeof brackets_loc);

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
