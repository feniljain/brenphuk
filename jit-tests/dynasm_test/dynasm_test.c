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
	| .arch x64
	| .define arg1Reg, rdi
	| .define arg2Reg, rsi
	| .define aux1Reg, r10
	| .define aux2Reg, r11
	| .define returnReg, rax

	|.actionlist actions

	dasm_State* d;
	dasm_State** Dst = &d;

	dasm_init(Dst, 1);

	dasm_setup(&d, actions);

	| .type state, exec_state_t, arg1Reg

	| mov aux1Reg, num
	| add aux1Reg, state:arg1Reg->num
	| add aux1Reg, state:arg1Reg->num1

	| push aux1Reg

	| mov arg2Reg, state:arg1Reg->put
	| mov arg1Reg, aux1Reg
	| call arg2Reg

	| pop returnReg

	| ret

	int (*fptr)(exec_state_t*) = link_and_encode(&d);
	return fptr;
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

	assert(ret == num + state.num + state.num1);

	return 0;
}

// [X] 1. normal integer use in jit code
// [X] 2. passing integer as function arg to add it with initial number
// [X] 3. passing a pointer to a number and adding that with initial number
// [X] 4. passing a function pointer and calling it to print new number
// [X] 5. passing a number in a struct and adding that with initial number
// [X] 6. passing a state struct with char and printing that with a passed function pointer
// [X] 7. use .define for regs
// [ ] 8. construct a dynamic jump point and jump to it
