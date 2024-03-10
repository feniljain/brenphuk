#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "debug.h"
#include "interpreter.h"

// could I use a hashmap here? Well for small programs and small
// number of brackets, a linear search will be faster (tsoding ftw)
void fill_brackets_loc(char *prog, int prog_len) {
  int i = 0, next_open_bracket_loc = -1;

  while (i < prog_len) {
    switch (prog[i]) {
    case '[': {
      int brackets_depth = 0;
      for (int j = i; j < prog_len; j++) {
        // OPTIMIZATION: make a stack and track all these []
        // encountered in between current pos: i and and it's corresponding
        // close bracket, then pop the stack and insert indexes into
        // *_bracket_loc
        if (prog[j] == '[') {
          if (next_open_bracket_loc == -1 && j != i) {
            next_open_bracket_loc = j;
          }
          brackets_depth++;
        } else if (prog[j] == ']') {
          brackets_depth--;
        }

        if (brackets_depth == 0) {
          open_brackets_loc[i] = j;
          close_brackets_loc[j] = i;
          break;
        }
      }

      if (brackets_depth != 0) {
        ABORT("brackets mismatch");
      }

      break;
    }
    default:
      break;
    }

    if (next_open_bracket_loc != -1) {
      // DBG_PRINTF("fill_brackets_loc::next_open_bracket_loc::i: %d", i);
      i = next_open_bracket_loc;
      next_open_bracket_loc = -1;
    } else {
      i++;
    }
  }
}

// enum Bracket { OPEN = 0, CLOSE = 1 };
// void print_bracket_arr(int stop_len, enum Bracket br) {
//   int i = 0;
//   char br_ch = '\0';
//   int *arr;
//
//   if (br == OPEN) {
//     br = '[';
//     arr = &open_brackets_loc[0];
//   } else {
//     br = ']';
//     arr = &close_brackets_loc[0];
//   }
//
//   while (i < TAPE_SIZE) {
//     if (stop_len != -1 && i > stop_len) {
//       break;
//     }
//
//     if (arr[i] != -1) {
//       DBG_PRINTF("brackets_loc[%d]: %c: %d", i, br_ch, arr[i]);
//     }
//
//     i += 1;
//   }
// }

void print_ops(void) {
  int i = 0;
  while (i < ops_len) {
    DBG_PRINTF("op[%d]: op_type: %d, repeat: %d", i, ops[i].op_type,
               ops[i].repeat);
    i++;
  }
}

void parse(char *prog, int prog_len) {
  int i = 0;

  while (i < prog_len) {
    enum Op_type op_type = INVALID;
    switch (prog[i]) {
    case '>':
      op_type = FWD;
    case '<':
      if (op_type == INVALID)
        op_type = BWD;
    case '+':
      if (op_type == INVALID)
        op_type = INCREMENT;
    case '-': {
      if (op_type == INVALID)
        op_type = DECREMENT;

      if (i > 0) {
        if (ops[i - 1].op_type == op_type) {
          ops[i].repeat++;
          i++;
          continue;
        }
      }

      break;
    }
    case '.':
      op_type = OUTPUT;
      break;
    case ',':
      op_type = INPUT;
      break;
    case '[':
      op_type = JMP_IF_ZERO;
      break;
    case ']':
      op_type = JMP_IF_NOT_ZERO;
      break;
    default:
      break;
    }
    Operation op;
    op.op_type = op_type;
    op.repeat = 1;

    ops[i] = op;
    ops_len++;

    i++;
  }

  print_ops();
}

// TODO(feniljain): shift all operations to functions and try flamegraph
int exec(char *prog, int prog_len) {
  DBG_PRINT(prog);
  int i = 0, b_idx; // , cnt = 0;

  // clock_t start, end;
  // double cpu_time_used;

  ops_len = 0;
  parse(prog, prog_len);
  fill_brackets_loc(prog, prog_len);
  // print_bracket_arr(-1, OPEN);

  while (i < prog_len) {
    // cnt++;
    // start = clock();
    switch (prog[i]) {
    case '>':
      b_idx = 0;
      pointer++; // TODO(feniljain): Add bound checks here
      break;
    case '<':
      b_idx = 1;
      pointer--; // TODO(feniljain): Add bound checks here
      break;
    case '+':
      b_idx = 2;
      tape[pointer]++; // I don't care about int overflow here
      break;
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
      b_idx = 6;
      if (tape[pointer] == 0) {
        int idx = open_brackets_loc[i];
        if (idx == -1) {
          DBG_PRINTF("[: got bracket_loc as -1 for i: %d", i);
          ABORT("invalid state");
        }
        i = idx;
        continue;
      }

      break;
    case ']': {
      b_idx = 7;
      if (tape[pointer] != 0) {
        int idx = close_brackets_loc[i];
        if (idx == -1) {
          DBG_PRINTF("]: got bracket_loc as -1 for i: %d", i);
          ABORT("invalid state");
        }
        i = idx;
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

  memset(open_brackets_loc, -1, sizeof open_brackets_loc);
  memset(close_brackets_loc, -1, sizeof close_brackets_loc);
}
