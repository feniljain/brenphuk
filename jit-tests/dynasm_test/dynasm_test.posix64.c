/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM x64 version 1.4.0
** DO NOT EDIT! The original file is in "dynasm_test.c".
*/

#line 1 "dynasm_test.c"
#include <stdio.h>
#include <sys/mman.h>

#include "luajit-2.0/dynasm/dasm_proto.h"
#include "luajit-2.0/dynasm/dasm_x86.h"

#include "debug.h"

typedef struct exec_state
{
  const char* str;
  void (*put_ch)(const char*);
} exec_state_t;

static void* link_and_encode(dasm_State** d) {
	size_t size;
	void *buf;
	dasm_link(d, &size);
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
	unsigned npc = 8; // number of dynamic labels involved

	//| .arch x64
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif
#line 40 "dynasm_test.c"

	//|.section code // specify what all different sections you need
#define DASM_SECTION_CODE	0
#define DASM_MAXSECTION		1
#line 42 "dynasm_test.c"
	dasm_init(&d, DASM_MAXSECTION);

	//|.globals lbl_
enum {
  lbl_start,
  lbl__MAX
};
#line 45 "dynasm_test.c"
	void* labels[lbl__MAX];
	dasm_setupglobal(&d, labels, lbl__MAX);

	//|.actionlist bf_actions
static const unsigned char bf_actions[17] = {
  254,0,248,10,255,72,139,191,233,255,252,255,151,233,255,195,255
};

#line 49 "dynasm_test.c"
	dasm_setup(&d, bf_actions);

	dasm_growpc(&d, npc);

	//| .type state, exec_state_t, rdi
#define Dt1(_V) (int)(ptrdiff_t)&(((exec_state_t *)0)_V)
#line 54 "dynasm_test.c"

	dasm_State** Dst = &d;
	//|.code
	dasm_put(Dst, 0);
#line 57 "dynasm_test.c"
	//|->start:
	dasm_put(Dst, 2);
#line 58 "dynasm_test.c"
	// | mov rax, 1
	// | mov rsi, 1
	//| mov rdi, state->str
	dasm_put(Dst, 5, Dt1(->str));
#line 61 "dynasm_test.c"
	// | mov rdx, 18

	//| call aword state->put_ch
	dasm_put(Dst, 10, Dt1(->put_ch));
#line 64 "dynasm_test.c"
	// | syscall

	//| ret
	dasm_put(Dst, 15);
#line 67 "dynasm_test.c"

	link_and_encode(&d);
	dasm_free(&d);
	return (void(*)(exec_state_t*))labels[lbl_start];
}

static void put_char(const char *s) {
	printf("%s", s);
}

int main() {
	exec_state_t state;

	state.str = "hello world\n";
	state.put_ch = put_char;
	exec()(&state);

	return 0;
}
