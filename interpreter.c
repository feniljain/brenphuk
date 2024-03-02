#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>

#include <readline/readline.h>

#define DBG_LOGS //comment this to disable debug comments

#ifdef DBG_LOGS
#define DBG_PRINT(l) fprintf(stderr, "DEBUG: %s\n", l);
#define DBG_PRINTF(fmt, ...) \
            fprintf(stderr, "DEBUG: " fmt "\n", __VA_ARGS__);
#else
#define DBG_PRINT(l) do {} while(0)
#define DBG_PRINTF(fmt, ...) \
            do {} while (0)
#endif

// #define assert(x) ((x) ? (void)0 : (printf("Assertion for %s @ %s:%d!\n", #x, __FILE__, __LINE__), abort()))

#define ABORT(l) fprintf(stderr, "unrecognized error: %s\n", l); abort();

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

const int STACK_SIZE = 1000; // 1000 nested for loops possible at max
const int TAPE_SIZE = 30000;

int main(int argc, char *argv[]);

// interpreter impl

typedef struct {
	int pointer;
	char tape[TAPE_SIZE];
	int brackets_loc[TAPE_SIZE][2];
} engine;

void print_bracket_arr(engine *eng, int stop_len) {
	int i = 0;

	while(true) {
		if(stop_len != -1) {
			if(i > stop_len) {
				break;
			}
		} else {
			if(eng->brackets_loc[i][0] == -1) {
				break;
			}
		}

		DBG_PRINTF("brackets_loc[%d]: [: %d ]: %d", i, eng->brackets_loc[i][0], eng->brackets_loc[i][1]);
		i += 1;
	}
}

void fill_brackets_loc(engine *eng, char *prog, int prog_len) {
	int i = 0, idx = 0;
	while(i < prog_len) {
		switch (prog[i]) {
			case '[': {
				eng->brackets_loc[idx][0] = i;

				int brackets_depth = 0;
				for(int j = i; j < prog_len; j++) {
					if(prog[j] == '[') {
						brackets_depth++;
					} else if(prog[j] == ']') {
						brackets_depth--;
					}
	
					if(brackets_depth == 0) {
						eng->brackets_loc[idx][1] = j;
						break;
					}
				}

				if(brackets_depth != 0) {
					ABORT("brackets mismatch");
				}

				idx++;
				break;
			}
			default: break;
		}

		i++;
	}
}

enum Bracket {OPEN = 1, CLOSE = 2};

int get_matching_bracket_idx(engine *eng, int idx, int br) {
	int check_idx = -1, ret_idx = -1;
	switch (br) {
		case OPEN: check_idx = 0; ret_idx = 1; break;
		case CLOSE: check_idx = 1; ret_idx = 0; break;
		default: ABORT("this is an internal function, what you passing dumbo"); break;
	}
	
	for (int i = 0; eng->brackets_loc[i][0] != -1; i++) {
		if(idx == eng->brackets_loc[i][check_idx]) {
			return eng->brackets_loc[i][ret_idx];
		}
	}

	return -1;
}
	
int exec(engine *eng, char *prog, int prog_len) {
	DBG_PRINT(prog);
	int i = 0;

	fill_brackets_loc(eng, prog, prog_len);
	print_bracket_arr(eng, -1);

	while(i < prog_len && i >= 0) {
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
			case '[':
			case ']':  {
				int br = OPEN;
				bool condition;
				if (prog[i] == '[') {
					condition = eng->tape[eng->pointer] == 0;
				} else {
					condition = eng->tape[eng->pointer] != 0;
					br = CLOSE;
				}

				if(condition) {
					int matching_idx = get_matching_bracket_idx(eng, i, br);
					if(matching_idx == -1) {
						ABORT("could not find matching bracket, bug in bracket finding code");
					}
					// DBG_PRINTF("] i: %d", i);
					i = matching_idx;
					continue;
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
	memset(eng->tape, 0, sizeof eng->tape);
	eng->pointer = 0;

	memset(eng->brackets_loc, -1, sizeof eng->brackets_loc);
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
			} else {
				int locs = atoi(cmd);
				for(int i = 0; i < locs; i++) {
					printf("%d: %d | ", i, eng->tape[i]);
				}
			}
			printf("\n");
		} else {
			int errno = exec(eng, prog, (int)strlen(prog));
			if(errno != 0) {printf("error: %s\n", strerr(errno));}
		}
		
		// add_history(); // TODO(feniljain): remember to check if it's an empty line, don't add it
		nxt_itr:
			free(prog);
	}

	return 0;
}

int tests(engine *eng) {
	exec(eng, "+++", 3);
	assert(eng->tape[0] == 3);

	reset(eng);
	DBG_PRINT("tests::test 1 done");

	// opening bracket for loop test
	exec(eng, "[++]", 4);
	assert(eng->tape[0] == 0);

	reset(eng);
	DBG_PRINT("tests::test 2 done");

	// nested opening bracket for loop test
	exec(eng, "[++[++]]", 8);
	assert(eng->tape[0] == 0);

	reset(eng);
	DBG_PRINT("tests::test 3 done");

	// closing bracket for loop test
	exec(eng, "+++[-]", 6);
	assert(eng->tape[0] == 0);

	reset(eng);
	DBG_PRINT("tests::test 4 done");

	exec(eng, ">+++++++++[<++++++>-]<...>++++++++++.", 37);
	assert(eng->tape[0] == 54);
	assert(eng->tape[1] == 10);

	reset(eng);
	DBG_PRINT("tests::test 5 done");

	// part of hello world
	exec(eng, "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]", 49);
	assert(eng->tape[0] == 0);
	assert(eng->tape[1] == 0);
	assert(eng->tape[2] == 72);
	assert(eng->tape[3] == 104);
	assert(eng->tape[4] == 88);
	assert(eng->tape[5] == 32);
	assert(eng->tape[6] == 8);

	reset(eng);
	DBG_PRINT("tests::test 6 done");

	// // hello world
	exec(eng, "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.", 106);

	reset(eng);
	DBG_PRINT("tests::test 7 done");

	return 0;
}

int main(int argc, char *argv[]) {
	if(argc != 1 && argc != 2) {
		printf("brenphuk [command]: repl, tests");
	}

	engine eng;
	memset(eng.tape, 0, sizeof eng.tape); // Make sure all tape is set to zero
	eng.pointer = 0;

	memset(eng.brackets_loc, -1, sizeof eng.brackets_loc);

	char* cmd = argv[1];
	if(cmd == NULL || !strcmp(cmd, "repl")) {
		return repl(&eng);
	} else if (!strcmp(cmd, "tests")) {
		tests(&eng);
	}

	return 0;
}
