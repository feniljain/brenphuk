# build:
# 	gcc -o bfret simple_interpreter.c \
# 	-Wall -Wextra -Werror -Weverything -pedantic -Wunused -Wuninitialized -Wshadow \
# 	-I/opt/homebrew/opt/readline/include \
# 	-L/opt/homebrew/opt/readline/lib \
# 	-lreadline

READLINE_LDFLAGS := -L/opt/homebrew/opt/readline/lib -lreadline
READLINE_CFLAGS := -I/opt/homebrew/opt/readline/include

bfret: simple_interpreter.o
	clang -o bfret simple_interpreter.o $(READLINE_LDFLAGS)

simple_interpreter.o: simple_interpreter.c
	clang \
	-Wall -Werror -Wextra -Weverything -pedantic -Wunused -Wuninitialized -Wshadow \
	-c simple_interpreter.c -o simple_interpreter.o \
	$(READLINE_CFLAGS) \

# -Wall -Werror -Wextra -Weverything -pedantic -Wunused -Wuninitialized -Wshadow \

# -c is used to compile without linking
# -I specifics where header files are located
# -L specifics where library files are located
# -l tells linker to link against readline lib
