/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM x64 version 1.4.0
** DO NOT EDIT! The original file is in "dynasm_test.c".
*/

#line 1 "dynasm_test.c"
// || (defined(_WIN32) != WIN) // windows? who supports that :P

// ||#if ((defined(_M_X64) || defined(__amd64__)) != X64)
// #error "wrong arch passed"

#include <stdio.h>
#include <assert.h>
#include <sys/mman.h>

#include "luajit-2.0/dynasm/dasm_proto.h"
#include "luajit-2.0/dynasm/dasm_x86.h"

#include "debug.h"

// debugging mode for dynasm
#define DASM_CHECKS 1 

typedef struct exec_state
{
  void (*put)(const char*);
  int num;
  int num1;
} exec_state_t;

static void* link_and_encode(dasm_State** d) {
	size_t size;

	int result = dasm_link(d, &size);
	assert(result == DASM_S_OK);

	char* buf = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	assert(buf != MAP_FAILED);

	dasm_encode(d, buf);
	dasm_free(d);

	result = mprotect(buf, size, PROT_EXEC | PROT_READ);
	assert(result == 0);

	FILE *fp = fopen("/tmp/jit-output", "w+");
	if(!fp) {
		ABORT("file not found");
	}
	fwrite(buf, size, 1, fp);
	fclose(fp);

	return buf;
}

static void* exec(int num) {
	//| .arch x64
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif
#line 52 "dynasm_test.c"
	//| .define arg1Reg, rdi
	//| .define arg2Reg, rsi
	//| .define aux1Reg, r10
	//| .define aux2Reg, r11
	//| .define returnReg, rax

	dasm_State* d;
	dasm_State** Dst = &d;

	dasm_init(Dst, 1);

	//| .globals lbl_
enum {
  lbl_start,
  lbl_before_call,
  lbl__MAX
};
#line 64 "dynasm_test.c"
	void* globals[lbl__MAX];
	dasm_setupglobal(&d, globals, lbl__MAX);

	//| .actionlist actions
static const unsigned char actions[32] = {
  248,10,73,199,194,237,255,76,3,151,233,255,65,82,255,72,139,183,233,76,137,
  215,248,11,252,255,214,255,88,255,195,255
};

#line 68 "dynasm_test.c"
	dasm_setup(&d, actions);

	//| .type state, exec_state_t, arg1Reg
#define Dt1(_V) (int)(ptrdiff_t)&(((exec_state_t *)0)_V)
#line 71 "dynasm_test.c"

	//| ->start:
	//| mov aux1Reg, num
	dasm_put(Dst, 0, num);
#line 74 "dynasm_test.c"

	if(num >= 7) {
		//| add aux1Reg, state:arg1Reg->num
		dasm_put(Dst, 7, Dt1(->num));
#line 77 "dynasm_test.c"
	} else {
		//| add aux1Reg, state:arg1Reg->num1
		dasm_put(Dst, 7, Dt1(->num1));
#line 79 "dynasm_test.c"
	}

	//| push aux1Reg
	dasm_put(Dst, 12);
#line 82 "dynasm_test.c"

	//| mov arg2Reg, state:arg1Reg->put
	//| mov arg1Reg, aux1Reg
	//| ->before_call:
	//| call arg2Reg
	dasm_put(Dst, 15, Dt1(->put));
#line 87 "dynasm_test.c"

	//| pop returnReg
	dasm_put(Dst, 28);
#line 89 "dynasm_test.c"

	//| ret
	dasm_put(Dst, 30);
#line 91 "dynasm_test.c"

	int (*fptr)(exec_state_t*) = link_and_encode(&d);

	// printf("before_call's offset: %d\n", dasm_getpclabel(&d, lbl_before_call));
	//return fptr;
	return (int (*)(exec_state_t*))globals[lbl_start];
	// return (int (*)(exec_state_t*))dasm_getpclabel(&d, lbl_start);
}

static void put(int num) {
	fprintf(stdout, "%d\n", num);
}

int main() {
	exec_state_t state;

	state.put = put;
	state.num = 7;
	state.num1 = 8;

	int num;
	printf("input num: \n", &num);
	scanf("%d", &num);

	int (*fptr)(exec_state_t*) = exec(num);
	int ret = fptr(&state);

	if(num >= 7) {
		assert(ret == num + state.num);
	} else {
		assert(ret == num + state.num1);
	}

	return 0;
}

// [X] 1. normal integer use in jit code
// [X] 2. passing integer as function arg to add it with initial number
// [X] 3. passing a pointer to a number and adding that with initial number
// [X] 4. passing a function pointer and calling it to print new number
// [X] 5. passing a number in a struct and adding that with initial number
// [X] 6. passing a state struct with char and printing that with a passed function pointer
// [X] 7. use .define for regs
// [X] 8. add static labels support
// [ ] 9. add dynamic labels support
// [ ] 10. construct a dynamic jump point (dynamic here means addr changes with different jit configs) and jump to it
