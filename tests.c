#include <assert.h>

#include "interpreter.h"
#include "debug.h"

int tests(void) {
  exec("+++", 3);
  assert(tape[0] == 3);

  reset();
  DBG_PRINT("tests::test 1 done");

  // opening bracket for loop test
  exec("[++]", 4);
  assert(tape[0] == 0);

  reset();
  DBG_PRINT("tests::test 2 done");

  // nested opening bracket for loop test
  exec("[++[++]]", 8);
  assert(tape[0] == 0);

  reset();
  DBG_PRINT("tests::test 3 done");

  // closing bracket for loop test
  exec("+++[-]", 6);
  assert(tape[0] == 0);

  reset();
  DBG_PRINT("tests::test 4 done");

  exec(">+++++++++[<++++++>-]<...>++++++++++.", 37);
  assert(tape[0] == 54);
  assert(tape[1] == 10);

  reset();
  DBG_PRINT("tests::test 5 done");

  // part of hello world
  exec("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]", 49);
  assert(tape[0] == 0);
  assert(tape[1] == 0);
  assert(tape[2] == 72);
  assert(tape[3] == 104);
  assert(tape[4] == 88);
  assert(tape[5] == 32);
  assert(tape[6] == 8);

  reset();
  DBG_PRINT("tests::test 6 done");

  // hello world
  exec("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>"
       ">.<-.<.+++.------.--------.>>+.>++.",
       106);

  reset();
  DBG_PRINT("tests::test 7 done");

  return 0;
}
