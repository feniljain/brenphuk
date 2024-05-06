/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM x64 version 1.4.0
** DO NOT EDIT! The original file is in "src/interpreter.c".
*/

#line 1 "src/interpreter.c"
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include "debug.h"
#include "interpreter.h"

#include "dasm_proto.h"
#include "dasm_x86.h"

// debugging mode for dynasm
#define DASM_CHECKS 1

// clang-format off
//|	.arch x64
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif
#line 24 "src/interpreter.c"
//|	.define arg1Reg, rdi
//|	.define arg2Reg, rsi
//|	.define aux1Reg, r10
//|	.define aux2Reg, r11
//|	.define returnReg, rax
    /* clang-format on */

    // =================== Types ===================

    int pointer;
char tape[TAPE_SIZE];

// OPTIMIZATION: Can also convert both of these arrs into one, by
// storing offset of other bracket, which will be the
// same for both [], just a matter of addition or
// subtraction to pointer, but for now it's fine as it is
int close_brackets_loc[TAPE_SIZE];
int open_brackets_loc[TAPE_SIZE];
// track iterations a loop has taken on open bracket pos
int loop_track[TAPE_SIZE];

enum Op_type {
  INVALID = 0,
  FWD,
  BWD,
  INCREMENT, // 3
  DECREMENT,
  OUTPUT,
  INPUT, // 6
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

uint8_t machine_code[TAPE_SIZE];
size_t codes_len = 0;

typedef struct {
  void (*put)(char c);
  void (*take)(char *c);
  char *tape_pointer;
} exec_state_t;

typedef char *(*func)(exec_state_t *);

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

void print_loop_track(void) {
  for (int i = 0; i < TAPE_SIZE; i++) {
    if (loop_track[i] > 0) {
      DBG_PRINTF("loop_track[%d]: %d", i, loop_track[i]);
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

static void link_and_encode(dasm_State **d) {
  size_t size;

  int result = dasm_link(d, &size);
  assert(result == DASM_S_OK);

  // NOTE: if you don't have any asm declared, this will fail with "invalid
  // argument"
  char *buf =
      mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  assert(buf != MAP_FAILED);

  dasm_encode(d, buf);
  dasm_free(d);

  result = mprotect(buf, size, PROT_EXEC | PROT_READ);
  assert(result == 0);

  DBG_PRINT("writing jit generated asm to file");

  FILE *fp = fopen("/tmp/jit-output", "w+");
  if (!fp) {
    ABORT("file not found");
  }
  fwrite(buf, size, 1, fp);
  fclose(fp);

  // return buf;
}

static void put_ch(char c) { fprintf(stderr, "%c", c); }

static void take_ch(char *c) { *c = (char)getchar(); }

func jit_loop(int start_idx, int end_idx) {
  int loop_lbl = 1, lbl_capacity = 108, loop_depth = 0, loop_lbls[TAPE_SIZE][2];

  dasm_State *d;
  dasm_State **Dst = &d;

  dasm_init(Dst, 1);

  // clang-format off

  //|.globals lbl_
enum {
  lbl_start,
  lbl__MAX
};
#line 291 "src/interpreter.c"
	void *globals[lbl__MAX];
  dasm_setupglobal(&d, globals, lbl__MAX);

	//| .actionlist actions
static const unsigned char actions[82] = {
  248,10,72,139,183,233,255,72,129,198,239,255,72,129,252,238,239,255,128,6,
  235,255,128,46,235,255,72,137,183,233,87,255,76,139,151,233,72,139,62,255,
  65,252,255,210,255,95,72,139,183,233,255,76,139,151,233,72,137,252,247,255,
  128,62,0,15,132,245,255,249,255,128,62,0,15,133,245,255,72,137,252,240,195,
  255
};

#line 295 "src/interpreter.c"
	dasm_setup(&d, actions);
	dasm_growpc(&d, lbl_capacity);

	//|.type state, exec_state_t, arg1Reg
#define Dt1(_V) (int)(ptrdiff_t)&(((exec_state_t *)0)_V)
#line 299 "src/interpreter.c"

  //| ->start:
	//| mov arg2Reg, state->tape_pointer
	dasm_put(Dst, 0, Dt1(->tape_pointer));
#line 302 "src/interpreter.c"

      // clang-format on

      for (int i = start_idx; i <= end_idx; i++) {
    switch (ops[i].op_type) {
    case INVALID: {
      ABORT("INVALID shouln't have leaked till here, there's a bug in parsing "
            "code");
    }
    case FWD: {
      // clang-format off
			//| add arg2Reg, ops[i].repeat
			dasm_put(Dst, 7, ops[i].repeat);
#line 314 "src/interpreter.c"
                     // clang-format on
                     break;
    }
    case BWD: {
      // clang-format off
			//| sub arg2Reg, ops[i].repeat
			dasm_put(Dst, 12, ops[i].repeat);
#line 320 "src/interpreter.c"
                     // clang-format on
                     break;
    }
    case INCREMENT: {
      // clang-format off
			//| add byte [arg2Reg], ops[i].repeat
			dasm_put(Dst, 18, ops[i].repeat);
#line 326 "src/interpreter.c"
                           // clang-format on
                           break;
    }
    case DECREMENT: {
      // clang-format off
			//| sub byte [arg2Reg], ops[i].repeat
			dasm_put(Dst, 22, ops[i].repeat);
#line 332 "src/interpreter.c"
                           // clang-format on
                           break;
    }
    case OUTPUT: {
      // clang-format off
			//| mov state:arg1Reg->tape_pointer, arg2Reg
			//| push arg1Reg
			dasm_put(Dst, 26, Dt1(->tape_pointer));
#line 339 "src/interpreter.c"

			//| mov aux1Reg, aword state:arg1Reg->put
			//| mov arg1Reg, [arg2Reg]
			dasm_put(Dst, 32, Dt1(->put));
#line 342 "src/interpreter.c"

			// | call aword state:arg1Reg->put
			//| call aux1Reg
			dasm_put(Dst, 40);
#line 345 "src/interpreter.c"

			//| pop arg1Reg
			//| mov arg2Reg, aword state:arg1Reg->tape_pointer
			dasm_put(Dst, 45, Dt1(->tape_pointer));
#line 348 "src/interpreter.c"
                        // clang-format on

                        break;
    }
    case INPUT: {
      // clang-format off
			//| mov state:arg1Reg->tape_pointer, arg2Reg
			//| push arg1Reg
			dasm_put(Dst, 26, Dt1(->tape_pointer));
#line 356 "src/interpreter.c"

			//| mov aux1Reg, aword state:arg1Reg->take
			//| mov arg1Reg, arg2Reg
			dasm_put(Dst, 51, Dt1(->take));
#line 359 "src/interpreter.c"

			//| call aux1Reg
			dasm_put(Dst, 40);
#line 361 "src/interpreter.c"

			//| pop arg1Reg
			//| mov arg2Reg, aword state:arg1Reg->tape_pointer
			dasm_put(Dst, 45, Dt1(->tape_pointer));
#line 364 "src/interpreter.c"
                        // clang-format on
                        break;
    }
    case JMP_IF_ZERO: {
      if ((loop_lbl + 2) >= lbl_capacity) {
        lbl_capacity *= 2;
        dasm_growpc(&d, lbl_capacity);
      }

      loop_lbls[loop_depth][0] = loop_lbl;
      loop_lbl++;

      loop_lbls[loop_depth][1] = loop_lbl;
      loop_lbl++;

      // DBG_PRINTF("JMP_IF_ZERO::loop_depth: %d, loop_lbl[0]: %d, loop_lbl[1]:
      // %d", loop_depth, loop_lbls[loop_depth][0], loop_lbls[loop_depth][1]);

      // clang-format off
			//| cmp byte [arg2Reg], 0
			//| jz =>loop_lbls[loop_depth][1]
			dasm_put(Dst, 60, loop_lbls[loop_depth][1]);
#line 385 "src/interpreter.c"

			//|=>loop_lbls[loop_depth][0]:
			dasm_put(Dst, 67, loop_lbls[loop_depth][0]);
#line 387 "src/interpreter.c"
          // clang-format on

          loop_depth++;

      break;
    }
    case JMP_IF_NOT_ZERO: {
      loop_depth--;

      // clang-format off
			//| cmp byte [arg2Reg], 0
			//| jnz =>loop_lbls[loop_depth][0]
			dasm_put(Dst, 69, loop_lbls[loop_depth][0]);
#line 399 "src/interpreter.c"

			//|=>loop_lbls[loop_depth][1]:
			dasm_put(Dst, 67, loop_lbls[loop_depth][1]);
#line 401 "src/interpreter.c"
          // clang-format on

          // DBG_PRINTF("JMP_IF_NOT_ZERO::loop_depth: %d, loop_lbl[0]: %d,
          // loop_lbl[1]: %d", loop_depth, loop_lbls[loop_depth][0],
          // loop_lbls[loop_depth][1]);

          break;
    }
    default: {
      break;
    }
    }
  }

  // clang-format off
  //| mov returnReg, arg2Reg
  //| ret
  dasm_put(Dst, 76);
#line 418 "src/interpreter.c"
                                 // clang-format on

                                 link_and_encode(&d);

  return (func)(globals[lbl_start]);

  // return (*(void **) (&func))(globals[lbl_start]);
}

int exec(char *prog, int prog_len) {
  DBG_PRINT(prog);
  int i = 0;
  uint8_t val;

  parse(prog, prog_len);
  // print_op_assoc(); // This is for checking which all ops occur together
  fill_brackets_loc();
  print_bracket_arr(-1, OPEN);

  func fptr = NULL;

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
      val = tape[pointer];
      val += ops[i].repeat; // TODO(feniljain): add int overflow check here
      tape[pointer] = val;
      break;
    case DECREMENT:
      val = tape[pointer];
      val -= ops[i].repeat; // TODO(feniljain): add int undeflow check here
      tape[pointer] = val;
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
      int idx = close_brackets_loc[i];

      if (tape[pointer] == 0) {
        if (idx == -1) {
          DBG_PRINTF("[: got bracket_loc as -1 for i: %d", i);
          ABORT("invalid state");
        }

        i = idx;
        break;
      } else {
        loop_track[i]++;
        if (loop_track[i] == 1) {
          fptr = jit_loop(i, idx);

          exec_state_t state;
          state.tape_pointer = &tape[pointer];
          state.put = put_ch;
          state.take = take_ch;

          char *ret_addr = fptr(&state);
          pointer = ret_addr - &tape[0];

          fptr = NULL;

          i = idx;
          break;
        }
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

  // print_loop_track();

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
  memset(loop_track, 0, sizeof loop_track);
}
