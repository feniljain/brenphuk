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

	| .arch x64

	|.section code // specify what all different sections you need
	dasm_init(&d, DASM_MAXSECTION);

	|.globals lbl_
	void* labels[lbl__MAX];
	dasm_setupglobal(&d, labels, lbl__MAX);

	|.actionlist bf_actions
	dasm_setup(&d, bf_actions);

	dasm_growpc(&d, npc);

	| .type state, exec_state_t, rdi

	dasm_State** Dst = &d;
	|.code
	|->start:
	// | mov rax, 1
	// | mov rsi, 1
	// | mov rdi, state->str
	// | mov rdx, 18

	| call aword state->put_ch
	// | syscall

	| ret

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
