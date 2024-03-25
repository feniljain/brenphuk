#include <iostream>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <sys/mman.h>

void append_msg_size(std::vector<uint8_t> &machine_code, const char *greeting) {
	size_t msg_size = strlen(greeting);
	machine_code[20] = ((msg_size >> 0) & 0xff);
	machine_code[21] = ((msg_size >> 8) & 0xff);
	machine_code[22] = ((msg_size >> 16) & 0xff);
	machine_code[23] = ((msg_size >> 24) & 0xff);
}

void print_machine_code(std::vector<uint8_t> &machine_code) {
	for(int i = 0; i < machine_code.size(); i++) {
			printf("mc[%d]: 0x%x\n", i, machine_code[i]);
	}
}

int main() {

	std::vector<uint8_t> machine_code {
		0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, // mov rax,0x1
		0x48, 0x89, 0xfe, // mov rsi,rdi
		0x48, 0xc7 ,0xc7, 0x01, 0x00, 0x00, 0x00, // mov rdi,0x1
		0x48, 0xc7, 0xc2, 0x12, 0x00, 0x00, 0x00, // mov rdx,0x12
		0x0f, 0x05, // syscall 
		0xc3, // ret
	};

	char greeting[] = "hello world";
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
