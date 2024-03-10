// NOTE: header guards, make import only happen once
// could use #pragma once too here, but that isn't
// a standard so skipping that part
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdint.h>

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
extern int ops_len;

// NOTE: int ops_len = 0; would throw duplicate sybmol error cause this var then
// gets included in every file which imports this header, instead if we just declare
// the variable here and we initialize it in some file, then only that file will
// contain this symbol, this happens even tho we have header guard in place

// =================== Functions ===================

int exec(char *prog, int prog_len);
void reset(void);

#endif
