#include <assert.h>

#include "debug.h"
#include "interpreter.h"

void simple_inc_test(void) {
  exec("+++", 3);
  assert(get_ele_at_idx(0) == 3);

  reset();
  DBG_PRINT("tests::simple_inc done");
}

void simple_dec_test(void) {
  exec("+++--", 5);
  assert(get_ele_at_idx(0) == 1);

  reset();
  DBG_PRINT("tests::simple_dec done");
}

void simple_ops_test(void) {
  exec("+++>++<--", 9);
  assert(get_ele_at_idx(0) == 1);
  assert(get_ele_at_idx(1) == 2);

  reset();
  DBG_PRINT("tests::simple_ops done");
}

void zero_itr_test(void) {
  // opening bracket for loop test
  exec("[++]", 4);
  assert(get_ele_at_idx(0) == 0);

  reset();
  DBG_PRINT("tests::zero_itr_test done");
}

void n_itr_test(void) {
  // opening bracket for loop test
  exec("+++[-]+", 7);
  assert(get_ele_at_idx(0) == 1);

  reset();
  DBG_PRINT("tests::zero_itr_test done");
}

void nested_open_bracket_test(void) {
  // nested opening bracket for loop test
  exec("[++[++]]", 8);
  assert(get_ele_at_idx(0) == 0);

  reset();
  DBG_PRINT("tests::test 3 done");
}

void close_bracket_test(void) {
  // closing bracket for loop test
  exec("+++[-]", 6);
  assert(get_ele_at_idx(0) == 0);

  reset();
  DBG_PRINT("tests::test 4 done");
}

void test_mandlebrot_start(void) {
  exec(">+++++++++[<++++++>-]<...>++++++++++.", 37);
  assert(get_ele_at_idx(0) == 54);
  assert(get_ele_at_idx(1) == 10);

  reset();
  DBG_PRINT("tests::test 5 done");
}

void test_part_hello_world(void) {
  // part of hello world
  exec("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]", 49);
  assert(get_ele_at_idx(0) == 0);
  assert(get_ele_at_idx(1) == 0);
  assert(get_ele_at_idx(2) == 72);
  assert(get_ele_at_idx(3) == 104);
  assert(get_ele_at_idx(4) == 88);
  assert(get_ele_at_idx(5) == 32);
  assert(get_ele_at_idx(6) == 8);

  reset();
  DBG_PRINT("tests::test 6 done");
}

void test_hello_world(void) {
  // hello world
  exec("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>"
       ">.<-.<.+++.------.--------.>>+.>++.",
       106);

  reset();
  DBG_PRINT("tests::test 7 done");
}

int tests(void) {
  simple_inc_test();
  simple_dec_test();
  simple_ops_test();
  zero_itr_test();
  n_itr_test();
  nested_open_bracket_test();
  close_bracket_test();
  test_mandlebrot_start();
  test_part_hello_world();
  test_hello_world();

  return 0;
}