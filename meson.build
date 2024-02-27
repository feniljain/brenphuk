project('brenphuk', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'default_library=static'])

readline_dep = dependency('readline').as_system()

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
