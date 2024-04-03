#include<stdio.h>

static void put(int num) {
	fprintf(stderr, "%d\n", num);
}

static void put_wrap(void (put)(int)) {
	put(5);
}

int main() {
	put_wrap(put);
	return 0;
}
