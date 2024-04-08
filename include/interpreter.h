// NOTE: header guards, make import only happen once
// could use #pragma once too here, but that isn't
// a standard so skipping that part
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdint.h>

#define TAPE_SIZE 30000

// NOTE: Initializations like int ops_len = 0; would throw duplicate sybmol
// error cause this var then gets included in every file which imports this
// header, instead if we just declare the variable here and we initialize it in
// some file, then only that file will contain this symbol, this happens even
// tho we have header guard in place

// =================== Functions ===================

int exec(char *prog, int prog_len);
void reset(void);

char get_curr_ele(void);
char get_ele_at_idx(int idx);
int get_pointer(void);

#endif
