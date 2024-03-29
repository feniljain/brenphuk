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

// =================== Types ===================

int pointer;
char tape[TAPE_SIZE];

// OPTIMIZATION: Can also convert both of these arrs into one, by
// storing offset of other bracket, which will be the
// same for both [], just a matter of addition or
// subtraction to pointer, but for now it's fine as it is
int close_brackets_loc[TAPE_SIZE];
int open_brackets_loc[TAPE_SIZE];

enum Op_type {
  INVALID = 0,
  FWD,
  BWD,
  INCREMENT,
  DECREMENT,
  OUTPUT,
  INPUT,
  JMP_IF_ZERO,
  JMP_IF_NOT_ZERO,
};

typedef struct {
  enum Op_type op_type;
  uint8_t repeat;
} Operation;
Operation ops[TAPE_SIZE];

int ops_len;

int op_assoc[9][9];

// =================== Getters ===================

char get_ele_at_idx(int idx) { return tape[idx]; }

char get_curr_ele(void) { return tape[pointer]; }

int get_pointer(void) { return pointer; }

// =================== Interpreter Impl ===================

enum Bracket { OPEN = 0, CLOSE = 1 };
void print_bracket_arr(int stop_len, enum Bracket br) {
  int i = 0;
  char br_ch = '\0';
  int *arr;

  if (br == OPEN) {
    br_ch = '[';
    arr = &close_brackets_loc[0];
  } else {
    br_ch = ']';
    arr = &open_brackets_loc[0];
  }

  while (i < TAPE_SIZE) {
    if (stop_len != -1 && i > stop_len) {
      break;
    }

    if (arr[i] != -1) {
      DBG_PRINTF("brackets_loc[%d]: %c: %d", i, br_ch, arr[i]);
    }

    i += 1;
  }
}

void print_ops(void) {
  for (int i = 0; i <= ops_len; i++) {
    DBG_PRINTF("op[%d]: op_type: %d, repeat: %d", i, ops[i].op_type,
               ops[i].repeat);
  }
}

void print_op_assoc(void) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (op_assoc[i][j]) {
        DBG_PRINTF("op_assoc[%d][%d]: %d", i, j, op_assoc[i][j]);
      }
    }
  }
}

void fill_brackets_loc(void) {
  for (int i = 0; i <= ops_len; i++) {
    if (ops[i].op_type != JMP_IF_ZERO) {
      continue;
    }

    int brackets_depth = 0;
    for (int j = i; j <= ops_len; j++) {
      // OPTIMIZATION: make a stack and track all these []
      // encountered in between current pos: i and and it's corresponding
      // close bracket, then pop the stack and insert indexes into
      // *_bracket_loc
      if (ops[j].op_type == JMP_IF_ZERO) {
        brackets_depth++;
      } else if (ops[j].op_type == JMP_IF_NOT_ZERO) {
        brackets_depth--;
      }

      if (brackets_depth == 0) {
        close_brackets_loc[i] = j;
        open_brackets_loc[j] = i;
        break;
      }
    }

    if (brackets_depth != 0) {
      ABORT("brackets mismatch");
    }
  }
}

void parse(char *prog, int prog_len) {
  int i = 0;

  while (i < prog_len) {
    bool repeat = false;
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

      if (ops_len >= 0) {
        if (ops[ops_len].op_type == op_type) {
          repeat = true;
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

    if (op_type == INVALID) {
      i++;
      continue; // this can happen when there are comments which are supposed to
                // be ignored
    }

    if (repeat) {
      ops[ops_len].repeat++;
      op_assoc[ops[ops_len].op_type][ops[ops_len].op_type]++;
    } else {
      Operation op;
      op.op_type = op_type;
      op.repeat = 1;

      ops[++ops_len] = op;
      if (ops_len > 0) {
        // This logic simply tries to unite op_assoc[1][5]
        // and op_assoc[5][1] into one single field
        int op_type_1 = (int)ops[ops_len].op_type;
        int op_type_2 = (int)ops[ops_len - 1].op_type;
        if (op_type_1 >= op_type_2) {
          op_assoc[op_type_2][op_type_1]++;
        } else {
          op_assoc[op_type_1][op_type_2]++;
        }
      }
    }

    i++;
  }
}

// TODO(feniljain): shift all operations to functions and try flamegraph
int exec(char *prog, int prog_len) {
  DBG_PRINT(prog);
  int i = 0, val;

  parse(prog, prog_len);
  // print_op_assoc(); // This is for checking which all ops occur together
  fill_brackets_loc();

  while (i <= ops_len) {
    // start = clock();
    switch (ops[i].op_type) {
    case FWD:
      pointer += ops[i].repeat; // TODO(feniljain): Add bound checks here
      break;
    case BWD:
      pointer -= ops[i].repeat; // TODO(feniljain): Add bound checks here
      break;
    case INCREMENT:
      val = (int)tape[pointer];
      val += ops[i].repeat; // TODO(feniljain): add int overflow check here
      tape[pointer] = (char)val;
      break;
    case DECREMENT:
      val = (int)tape[pointer];
      val -= ops[i].repeat; // TODO(feniljain): add int undeflow check here
      tape[pointer] = (char)val;
      break;
    case OUTPUT:
      printf("%c", tape[pointer]);
      break;
    case INPUT: {
      char ch = (char)getchar();
      tape[pointer] = ch;
      break;
    }
    case JMP_IF_ZERO: {
      if (tape[pointer] == 0) {
        int idx = close_brackets_loc[i];
        if (idx == -1) {
          DBG_PRINTF("[: got bracket_loc as -1 for i: %d", i);
          ABORT("invalid state");
        }
        i = idx;
        continue;
      }

      break;
		}
    case JMP_IF_NOT_ZERO: {
      if (tape[pointer] != 0) {
        int idx = open_brackets_loc[i];
        if (idx == -1) {
          DBG_PRINTF("]: got bracket_loc as -1 for i: %d", i);
          ABORT("invalid state");
        }
        i = idx;
        continue;
      }

      break;
    }
    case INVALID:
      ABORT("INVALID shouln't have leakded till here, there's a bug in parsing "
            "code");
    default:
      break;
    }

    i++;

    // end = clock();
    // cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    // benchmark_results[b_idx][0]++;
    // benchmark_results[b_idx][1] += cpu_time_used;
  }

  // print_benchmark_results();
  return 0;
}

void reset(void) {
  memset(tape, 0, sizeof tape);
  pointer = 0;

  ops_len = -1;

  memset(op_assoc, 0, sizeof op_assoc);
  memset(close_brackets_loc, -1, sizeof close_brackets_loc);
  memset(open_brackets_loc, -1, sizeof open_brackets_loc);
}
