project(
				'brenphuk', 'c',
				version : '0.1',
				default_options : ['warning_level=3', 'default_library=static'],
)

readline_dep = dependency('readline').as_system()

inc = include_directories('./include')

# dynasm_dep = declare_dependency(include_directories('./dynasm-doc')).as_system()

# source: https://github.com/tiernemi/meson-sample-project/blob/master/meson.build
# This adds the clang format file to the build directory
configure_file(
							input : '.clang-format',
							output : '.clang-format',
							copy: true)

# TODO: search for clang-format path and use them here, making it work across machines
run_target('format',
  command : ['clang-format-13','-i','-style=file src/* include/*'])

run_command('clang-format-13','-i','-style=file src/* include/*', check: true)

run_command('minilua', './dynasm-doc/luajit-2.0/dynasm/dynasm.lua','-o', 'src/interpreter.posix64.c', '-D', 'X64', 'src/interpreter.c', check: true)

executable('brenphuk',
						'src/main.c',
						'src/tests.c',
						'src/repl.c',
						'src/interpreter.posix64.c',
						'src/benchmark.c',
						install : true,
						c_args: [
								'-Werror', '-Wall', '-Wextra', '-Wshadow', '-Wconversion', 
								'-Wcast-align', '-Wunused', '-Wpointer-arith',
								'-Wundef', '-Winit-self', '-Wredundant-decls', '-Wmissing-include-dirs', 
								'-Wswitch-default', '-Wswitch-enum', '-Wfloat-equal', '-Wformat-security', 
								'-Wno-implicit-fallthrough',
								'-Wno-pedantic',  # can't cast void* as function pointer without this :sob:, try this someday: https://stackoverflow.com/questions/14134245/iso-c-void-and-function-pointers
								'-Wno-sign-conversion', '-Wno-conversion', '-Wno-switch-default', # this row is for no's to get dynasm header files compiled
								# '-O3', # uncomment when done with JIT impl, commenting for simplification looking at final code when debugging JIT bugs
								# '-Db_sanitize=address,undefined',
								'-g'
						],
						include_directories: inc,
						dependencies : [readline_dep] # , dynasm_dep],
)
