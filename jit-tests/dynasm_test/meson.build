project('dynasm_test', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'default_library=static'])

inc = include_directories('../../dynasm-doc', '../../')

# source: https://github.com/tiernemi/meson-sample-project/blob/master/meson.build
# This adds the clang format file to the build directory
configure_file(input : '.clang-format',
               output : '.clang-format',
							 copy: true)

run_target('format',
  command : ['clang-format-13','-i','-style=file', ['dynasm_test.c']])

run_command('clang-format-13','-i','-style=file', 'dynasm_test.c')

run_command('./minilua', '../dynasm-doc/luajit-2.0/dynasm/dynasm.lua','-o', 'dynasm_test.posix64.c', '-D', 'X64', 'dynasm_test.c', check: true)

# TODO: Add all cflags
executable('dynasm_test',
           'dynasm_test.posix64.c',
           install : true,
           include_directories: inc,
 					 c_args: ['-Wno-implicit-fallthrough', '-g'],
)
