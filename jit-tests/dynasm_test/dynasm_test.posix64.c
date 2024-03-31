/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM x64 version 1.4.0
** DO NOT EDIT! The original file is in "dynasm_test.c".
*/

#line 1 "dynasm_test.c"
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
	//|.arch x64
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif
#line 46 "dynasm_test.c"
	//|.actionlist actions
static const unsigned char actions[4] = {
  184,237,195,255
};

#line 47 "dynasm_test.c"

	dasm_State* d;
	dasm_State** Dst = &d;

	dasm_init(Dst, 1);

	dasm_setup(&d, actions);

	// |.define aState, r12
	// |.type state, exec_state_t, aState

	// assert(dasm_checkstep(Dst, 0) == 0);

	// assert(dasm_checkstep(Dst, 0) == 0);
	// |->start:
	// assert(dasm_checkstep(Dst, 0) == 0);

	// | mov rdi, state->str
	// | mov rdi, aState
	// | call aword state->put
	// assert(dasm_checkstep(Dst, 0) == 0);

	//| mov eax, num // TODO: try converting this to use struct state
	//| ret
	dasm_put(Dst, 0, num);
#line 71 "dynasm_test.c"
	// assert(dasm_checkstep(Dst, 0) == 0);

	int (*fptr)(exec_state_t*) = link_and_encode(&d);
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
	int (*fptr)() = exec(state.num);
	int ret = fptr(&state);

	assert(ret == state.num);

	return 0;
}
