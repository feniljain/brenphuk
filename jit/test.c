#include <stdio.h>
#include <limits.h>

void print_bin(char integer)
{
    int i = CHAR_BIT * sizeof(char); /* however many bits are in an integer */
    while(i--) {
        putchar('0' + ((integer >> i) & 1)); 
    }
}

int main() {
	// printf("value: %b\n", ());
	print_bin(a);
	return 0;
}
