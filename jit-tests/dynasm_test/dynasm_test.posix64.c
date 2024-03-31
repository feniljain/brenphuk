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
  const char* str;
  void (*put)(const char*);
} exec_state_t;

static void* link_and_encode(dasm_State** d) {
	size_t size;
	void *buf;
	assert(dasm_link(d, &size) == 0);
	buf = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	dasm_encode(d, buf);

	FILE *fp = fopen("/tmp/jit-output", "w+");
	if(!fp) {
		ABORT("file not found");
	}

	fwrite(buf, size, 1, fp);

	fclose(fp);

	mprotect(buf, size, PROT_READ | PROT_EXEC);
	return buf;
}

static void(* exec()) (exec_state_t*) {
	dasm_State* d;

	//|.arch x64
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif
#line 42 "dynasm_test.c"

	dasm_init(&d, 1);

	//|.globals lbl_
enum {
  lbl_start,
  lbl__MAX
};
#line 46 "dynasm_test.c"
	void* labels[lbl__MAX];
	dasm_setupglobal(&d, labels, lbl__MAX);

	//|.actionlist actions
static const unsigned char actions[16] = {
  248,10,255,76,137,231,65,252,255,148,253,36,233,255,195,255
};

#line 50 "dynasm_test.c"
	dasm_setup(&d, actions);

	//|.define aState, r12
	//|.type state, exec_state_t, aState
#define Dt1(_V) (int)(ptrdiff_t)&(((exec_state_t *)0)_V)
#line 54 "dynasm_test.c"

	// assert(dasm_checkstep(Dst, 0) == 0);

	dasm_State** Dst = &d;
	// assert(dasm_checkstep(Dst, 0) == 0);
	//|->start:
	dasm_put(Dst, 0);
#line 60 "dynasm_test.c"
	// assert(dasm_checkstep(Dst, 0) == 0);

	// | mov rdi, state->str
	//| mov rdi, aState
	//| call aword state->put
	dasm_put(Dst, 3, Dt1(->put));
#line 65 "dynasm_test.c"
	// assert(dasm_checkstep(Dst, 0) == 0);

	//| ret
	dasm_put(Dst, 14);
#line 68 "dynasm_test.c"
	// assert(dasm_checkstep(Dst, 0) == 0);

	link_and_encode(&d);
	dasm_free(&d);
	return (void(*)(exec_state_t*))labels[lbl_start];
}

static void put(const char *s) {
	printf("%s", s);
}

int main() {

	exec_state_t state;

	state.str = "hello world\n";
	state.put = &put;

	// put(state.str);
	exec()(&state);

	return 0;
}
