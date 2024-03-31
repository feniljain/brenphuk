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

	|.arch x64

	dasm_init(&d, 1);

	|.globals lbl_
	void* labels[lbl__MAX];
	dasm_setupglobal(&d, labels, lbl__MAX);

	|.actionlist actions
	dasm_setup(&d, actions);

	|.define aState, r12
	|.type state, exec_state_t, aState

	// assert(dasm_checkstep(Dst, 0) == 0);

	dasm_State** Dst = &d;
	// assert(dasm_checkstep(Dst, 0) == 0);
	|->start:
	// assert(dasm_checkstep(Dst, 0) == 0);

	// | mov rdi, state->str
	| mov rdi, aState
	| call aword state->put
	// assert(dasm_checkstep(Dst, 0) == 0);

	| ret
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
	state.put = put;

	// put(state.str);
	exec()(&state);

	return 0;
}
