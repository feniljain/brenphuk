project('brenphuk', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'default_library=static'])

readline_dep = dependency('readline').as_system()

inc = include_directories('./include')
# dynasm_dep = declare_dependency(include_directories('./dynasm-doc')).as_system()

# source: https://github.com/tiernemi/meson-sample-project/blob/master/meson.build
# This adds the clang format file to the build directory
configure_file(input : '.clang-format',
               output : '.clang-format',
	       copy: true)

# TODO: search for clang-format path and use them here, making it work across machines
run_target('format',
  command : ['clang-format-13','-i','-style=file', ['../interpreter.c']])

run_command('clang-format-13','-i','-style=file', 'interpreter.c')

executable('brenphuk',
           'main.c',
           'tests.c',
           'repl.c',
           'interpreter.c',
           'benchmark.c',
           install : true,
					 c_args: [
						'-Werror', '-Wall', '-Wextra', '-Wshadow', '-Wconversion', 
						'-Wcast-align', '-Wunused', '-Wpointer-arith',
						'-Wundef', '-Winit-self', '-Wredundant-decls', '-Wmissing-include-dirs', 
						'-Wswitch-default', '-Wswitch-enum', '-Wfloat-equal', '-Wformat-security', 
						'-Wno-implicit-fallthrough',
						'-Wpedantic',
						# '-O3', # uncomment when done with JIT impl, commenting for simplification looking at final code when debugging JIT bugs
						# '-Db_sanitize=address,undefined',
						'-g'
					],
					include_directories: inc,
					dependencies : [readline_dep] # , dynasm_dep],
)
