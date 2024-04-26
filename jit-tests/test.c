#include <stdio.h>

static void put(int* c) {
	fprintf(stderr, "%c\n", *c);
}

// static void put_wrap(void (put)(int)) {
// 	put(5);
// }

int main() {
	// put_wrap(put);
	int a = 65;
	put(&a);
	return 0;
}
