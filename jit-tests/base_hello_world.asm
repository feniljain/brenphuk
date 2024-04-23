# ssize_t write(int fd, const void *buf, size_t count);

mov $1, %rax # write syscall number
mov %rdi, %rsi # second arg to write syscall
mov $1, %rdi # first arg to write syscall
mov $18, %rdx # third arg to write syscall
syscall

ret

.string "hello, your name\n"
