#include <stdio.h>
#include <assert.h>
#include <sys/mman.h>

#include "luajit-2.0/dynasm/dasm_proto.h"
#include "luajit-2.0/dynasm/dasm_x86.h"

#include "debug.h"

#define DASM_CHECKS 1 // debugging mode for dynasm

typedef struct exec_state
{
  // const char* str;
  // void (*put)(const char*);
  int num;
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
	|.arch x64
	|.actionlist actions

	dasm_State* d;
	dasm_State** Dst = &d;

	dasm_init(Dst, 1);

	dasm_setup(&d, actions);

	// |.define aState, r12
	// |.type state, exec_state_t, rdi

	// assert(dasm_checkstep(Dst, 0) == 0);
	// |->start:
	// assert(dasm_checkstep(Dst, 0) == 0);

	// | mov rdi, state->str
	// | mov rdi, aState
	// | call aword state->put
	// assert(dasm_checkstep(Dst, 0) == 0);

	| mov rdi, [rdi]
	| add rdi, num
	| mov rax, rdi
	| ret
	// assert(dasm_checkstep(Dst, 0) == 0);

	int (*fptr)(int*) = link_and_encode(&d);
	return fptr;
	// return (void(*)(exec_state_t*))labels[lbl_start];
}

// static void put(const char* s) {
// 	printf("%s", s);
// }

int main() {
	exec_state_t state;

	// state.str = "hello world\n";
	// state.put = put;

	state.num = 7;
	// put(state.str);
	int (*fptr)(int*) = exec(state.num);
	int ret = fptr(&state.num);

	assert(ret == state.num * 2);

	return 0;
}

// [X] 1. normal integer use in jit code
// [X] 2. passing integer as function arg to add it with initial number
// [X] 3. passing a pointer to a number and adding that with initial number
// [ ] 4. passing a function pointer and calling it to print new number
// [ ] 5. passing a number in a struct and adding that with initial number
// [ ] 6. passing a state struct with char and printing that with a passed function pointer
