#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>

#include <readline/readline.h>

#ifdef DBG_LOGS
#define DBG_PRINT(l) fprintf(stderr, "DEBUG: %s\n", l);
#define DBG_PRINTF(fmt, ...) \
            fprintf(stderr, "DEBUG: " fmt "\n", __VA_ARGS__);
#else
#define DBG_PRINT(l) do {} while(0)
#define DBG_PRINTF(fmt, ...) \
            do {} while (0)
#endif

#define ABORT(l) fprintf(stderr, "unrecognized error: %s\n", l); abort();

const int STACK_SIZE = 1000; // 1000 nested for loops possible at max
const int TAPE_SIZE = 30000;

int main(int argc, char *argv[]);

// ======================================

// error handling
enum Error {ERR_NO_MATCHING_BRACKET = 1};

char* strerr(int errnum) {
	switch(errnum) {
		case ERR_NO_MATCHING_BRACKET: return "did not find matching loop bracket";
		default: ABORT("incorrect error num passed");
	}
}

// ======================================

// ======================================

// stack impl
typedef struct {
	int idx;
	char buf[STACK_SIZE][100];
} stack;

void push(stack *st, char* ele, int *ok) {
	if(st->idx >= STACK_SIZE) {
		if(ok) {*ok = -1; return;}
	}

	memcpy(st->buf[st->idx], &ele, 100);
	st->idx++;
}

char* pop(stack *st, int *ok) {
	if(st->idx < 0) { // TODO: Check if this can be coverted to a generic bounds check preprocessor
		if(ok) {*ok = -1; return "";}
	}

	st->idx--;

	return &st->buf[st->idx + 1][0];
}

char* peek(stack *st, int *ok) {
	if(st->idx < 0 || st->idx > STACK_SIZE) {
		if(ok) {*ok = -1; return "";}
	}

	return &st->buf[st->idx][0];
}

void print_stack(stack *st) {
	if(st->idx == 0) {printf("stack is empty");}
	for(int i = 0; i < st->idx; i++) {
		printf("%s | ", st->buf[i]);
	}
}

// ======================================

// interpreter impl

typedef struct {
	char tape[TAPE_SIZE]; // TODO(feniljain): convert it into an array of 1 byte elements
	int pointer;
	bool capture_loop_str;
	char loop_str[100];
	stack st;
} engine;

int exec(engine *eng, char *prog) {
	DBG_PRINT(prog);
	size_t prog_len = strlen(prog);
	size_t i = 0;
	while(i < prog_len) {
		switch(prog[i]) {
			case '>':  eng->pointer++; break; // TODO(feniljain): Add bound checks here
			case '<':  eng->pointer--; break; // TODO(feniljain): Add bound checks here
			case '+':  eng->tape[eng->pointer]++; break; // I don't care about int overflow here
			case '-':  eng->tape[eng->pointer]--; break;
			case '.':  printf("%c\n", eng->tape[eng->pointer]); break; 
			case ',':  {
				char ch; 
				scanf("%c", &ch);
				eng->tape[eng->pointer]=ch;
				break;
			}
			case '[':  {
				if(eng->tape[eng->pointer] == 0) {
					int brackets_depth = 0;
					while(i < prog_len) {
						DBG_PRINTF("[ prog[%zu]: %c %d", i, prog[i], brackets_depth);

						if(prog[i] == '[') {
							brackets_depth++;
						} else if (prog[i] == ']') {
							brackets_depth--;
						}

						if(!brackets_depth) {
							break;
						}

						i++;
					}

					if(brackets_depth != 0) {
						ABORT("could not find matching closing square bracket");
					}	
				}

				break;
			}
			case ']':  {
				if(eng->tape[eng->pointer] != 0) {
					int brackets_depth = 0;
					while(i < prog_len) {
						DBG_PRINTF("] prog[%zu]: %c %d", i, prog[i], brackets_depth);

						if(prog[i] == '[') {
							brackets_depth--;
						} else if (prog[i] == ']') {
							brackets_depth++;
						}

						if(!brackets_depth) {
							break;
						}

						i--;
					}

					if(brackets_depth != 0) {
						ABORT("could not find matching opening square bracket");
					}	
				}

				break;
			}
			default: break;
		}

		i++;
	}

	return 0;
}

void reset(engine *eng) {
	memset(eng->tape, 0, TAPE_SIZE * sizeof(eng->tape[0]));
	eng->pointer = 0;

	stack st;
	st.idx = 0;
	eng->st = st;
}

int repl(engine *eng) {
	printf("Brainfuck Repl\n");
	for(;;) {
		char *prog = (char*)NULL;
		prog = readline(">> ");

		if(!strcmp(prog, "exit")) {
			break;
		} else if(!strcmp(prog, "")) {
			goto nxt_itr;
		} else if(!strcmp(prog, "reset")) {
			reset(eng);
		} else if(strstr(prog, "print") != NULL) {
			strsep(&prog, " "); // print cmd

			char* cmd = strsep(&prog, " ");

			if(!strcmp(cmd, "idx")) {
				printf("pointer index: %d", eng->pointer);
			} else if (!strcmp(cmd, "curr")) {
				printf("value at current index: %d", eng->tape[eng->pointer]);
			} else if (!strcmp(cmd, "loop_str")) {
				printf("loop string: %s", eng->loop_str);
			} else {
				int locs = atoi(cmd);
				for(int i = 0; i < locs; i++) {
					printf("%d: %d | ", i, eng->tape[i]);
				}
			}
			printf("\n");
		} else {
			int errno = exec(eng, prog);
			if(errno != 0) {printf("error: %s\n", strerr(errno));}
			
			if(errno == ERR_NO_MATCHING_BRACKET) {return 1;} // end program
		}
		
		// add_history(); // TODO(feniljain): remember to check if it's an empty line, don't add it
		nxt_itr:
			free(prog);
	}

	return 0;
}

int tests(engine *eng) {
	exec(eng, "+++");
	assert(eng->tape[0] == 3);

	reset(eng);
	DBG_PRINT("tests::test 1 done");

	// opening bracket for loop test
	exec(eng, "[++]");
	assert(eng->tape[0] == 0);

	reset(eng);
	DBG_PRINT("tests::test 2 done");

	// nested opening bracket for loop test
	exec(eng, "[++[++]]");
	assert(eng->tape[0] == 0);

	reset(eng);
	DBG_PRINT("tests::test 3 done");

	// closing bracket for loop test
	exec(eng, "+++[-]");
	assert(eng->tape[0] == 0);

	reset(eng);
	DBG_PRINT("tests::test 4 done");

	// closing bracket for loop test
	exec(eng, "+++>+[[-]<-]");
	assert(eng->tape[0] == 0);
	assert(eng->tape[1] == 0);

	reset(eng);
	DBG_PRINT("tests::test 5 done");

	// part of hello world
	exec(eng, "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]");
	assert(eng->tape[0] == 0);
	assert(eng->tape[1] == 0);
	assert(eng->tape[2] == 72);
	assert(eng->tape[3] == 104);
	assert(eng->tape[4] == 88);
	assert(eng->tape[5] == 32);
	assert(eng->tape[6] == 8);

	reset(eng);
	DBG_PRINT("tests::test 6 done");

	// part of hello world
	exec(eng, "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]");
	assert(eng->tape[0] == 0);
	assert(eng->tape[1] == 0);
	assert(eng->tape[2] == 72);
	assert(eng->tape[3] == 104);
	assert(eng->tape[4] == 88);
	assert(eng->tape[5] == 32);
	assert(eng->tape[6] == 8);

	reset(eng);
	DBG_PRINT("tests::test 7 done");

	// hello world
	exec(eng, "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.");

	reset(eng);
	DBG_PRINT("tests::test 8 done");

	return 0;
}

int main(int argc, char *argv[]) {
	if(argc != 1 && argc != 2) {
		printf("brenphuk [command]: repl, tests");
	}

	engine eng;
	memset(eng.tape, 0, TAPE_SIZE * sizeof(eng.tape[0])); // Make sure all tape is set to zero
	eng.pointer = 0;

	stack st;
	st.idx = 0;
	eng.st = st;

	char* cmd = argv[1];
	if(cmd == NULL || !strcmp(cmd, "repl")) {
		return repl(&eng);
	} else if (!strcmp(cmd, "tests")) {
		tests(&eng);
	}

	return 0;
}
