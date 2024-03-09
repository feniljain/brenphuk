// NOTE: header guards, make import only happen once
// could use #pragma once too here, but that isn't
// a standard so skipping that part
#ifndef INTERPRETER_H
#define INTERPRETER_H

// =================== Types ===================
#define TAPE_SIZE 30000

int pointer;
char tape[TAPE_SIZE];
// OPTIMIZATION: Can also convert both of these arrs into one, by
// storing offset of other bracket, which will be the
// same for both [], just a matter of addition or
// subtraction to pointer, but for now it's fine as it is
int open_brackets_loc[TAPE_SIZE];
int close_brackets_loc[TAPE_SIZE];

// enum Op_type {
//   FWD = 0,
//   BWD = 1,
//   INCREMENT = 2,
//   DECREMENT = 3,
//   OUTPUT = 4,
//   INPUT = 5,
//   JMP_IF_ZERO = 5,
//   JMP_IF_NOT_ZERO = 6,
// };
//
// typedef struct {
//   enum Op_type op_type;
//   uint8_t repeat;
// } Operation;
// Operation cmds[TAPE_SIZE];

// =================== Functions ===================

int exec(char *prog, int prog_len);
void reset(void);

#endif
