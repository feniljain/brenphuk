project('brenphuk', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'default_library=static'])

readline_dep = dependency('readline').as_system()

# source: https://github.com/tiernemi/meson-sample-project/blob/master/meson.build
# This adds the clang format file to the build directory
configure_file(input : '.clang-format',
               output : '.clang-format',
	       copy: true)

run_target('format',
  command : ['clang-format','-i','-style=file', ['../interpreter.c']])

run_command('clang-format','-i','-style=file', 'interpreter.c')

executable('brenphuk',
           'interpreter.c',
           install : true,
		   c_args: ['-Werror', '-Wall', '-Wextra', '-Wshadow', '-Wconversion', 
					'-Wcast-align', '-Wunused', '-Wpointer-arith', '-Wold-style-cast', 
					'-Wundef', '-Winit-self', '-Wredundant-decls', '-Wmissing-include-dirs', 
					'-Wswitch-default', '-Wswitch-enum', '-Wfloat-equal', '-Wformat-security', 
					'-Wpedantic'],
           dependencies : [readline_dep],
)
