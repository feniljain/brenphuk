#include <iostream>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <sys/mman.h>

void append_msg_size(std::vector<uint8_t> &machine_code, const char *greeting) {
	size_t msg_size = strlen(greeting);
	machine_code[14] = ((msg_size >> 0) & 0xff);
	machine_code[15] = ((msg_size >> 8) & 0xff);
	machine_code[16] = ((msg_size >> 16) & 0xff);
	machine_code[17] = ((msg_size >> 24) & 0xff);
}

void print_machine_code(std::vector<uint8_t> &machine_code) {
	for(int i = 0; i < machine_code.size(); i++) {
			printf("mc[%d]: 0x%x\n", i, machine_code[i]);
	}
}

int main() {
	std::vector<uint8_t> machine_code {
		0xb8, 0x01, 0x00, 0x00, 0x00,
		0x48, 0x89, 0xfe,
		0xbf, 0x01, 0x00, 0x00, 0x00,
		0xba, 0x12, 0x00, 0x00, 0x00,

		// add one to first char ascii value
		// in case of hello, world, it should print:
		// iello world
		0x48, 0x8b, 0x0e,
		0x48, 0x83, 0xc1, 0x01,
		0x48, 0x89, 0x0e,

		0x0f, 0x05,
		0xc3,
	};

	char greeting[] = "hello world\n";
	append_msg_size(machine_code, greeting);

	// print_machine_code(machine_code);

	uint8_t *mem = (uint8_t*) mmap(NULL, machine_code.size(), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS ,-1, 0);
	if(mem == MAP_FAILED) {
		printf("can't allocate memory");
		return 1;
	}
		
	for(size_t i = 0; i < machine_code.size(); i++) {
		mem[i] = machine_code[i];
	}

	void (*func)(char*);
	func = (void (*)(char*))mem;
	func(greeting);

	munmap(mem, machine_code.size());

	return 0;
}
